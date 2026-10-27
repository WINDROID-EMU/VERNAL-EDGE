// Decompile specific functions that failed or timed out during bulk run.
// Reads exe_decompiled/failed.txt, decompiles each with a generous timeout and fallback styles.
//@category YYC
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import java.io.*;
import java.util.*;

public class DecompileFailed extends GhidraScript {

    @Override
    public void run() throws Exception {
        File baseDir = new File("exe_decompiled");
        File failedList = new File(baseDir, "failed.txt");
        File gmlDir = new File(baseDir, "gml");
        gmlDir.mkdirs();

        if (!failedList.exists()) {
            println("failed.txt not found at " + failedList.getAbsolutePath());
            return;
        }

        List<String[]> targets = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(failedList))) {
            String line;
            while ((line = br.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) continue;
                String[] parts = line.split("\t");
                if (parts.length >= 2) {
                    targets.add(new String[]{parts[0].trim(), parts[1].trim()});
                }
            }
        }

        println("Found " + targets.size() + " target functions to re-decompile.");

        DecompInterface di = new DecompInterface();
        DecompileOptions options = new DecompileOptions();
        options.setMaxPayloadMBytes(128);
        di.setOptions(options);
        di.toggleCCode(true);
        di.toggleSyntaxTree(false);
        di.setSimplificationStyle("decompile");
        di.openProgram(currentProgram);

        AddressFactory af = currentProgram.getAddressFactory();
        FunctionManager fm = currentProgram.getFunctionManager();

        int success = 0;
        int stillFailed = 0;

        File retryLog = new File(baseDir, "retry_results.tsv");
        try (PrintWriter pwLog = new PrintWriter(new FileWriter(retryLog))) {
            pwLog.println("Address\tName\tStatus\tStyleUsed\tOutputFile");

            for (int i = 0; i < targets.size(); i++) {
                String addrStr = targets.get(i)[0];
                String name = targets.get(i)[1];
                println(String.format("[%d/%d] Processing %s (%s)...", (i + 1), targets.size(), name, addrStr));

                Address addr = af.getAddress(addrStr);
                if (addr == null) {
                    println("  ERROR: could not parse address " + addrStr);
                    stillFailed++;
                    continue;
                }

                Function f = fm.getFunctionAt(addr);
                if (f == null) {
                    f = fm.getFunctionContaining(addr);
                }
                if (f == null) {
                    println("  ERROR: function not found at " + addr);
                    stillFailed++;
                    continue;
                }

                // 1. Try full "decompile" with 300s timeout
                di.setSimplificationStyle("decompile");
                DecompileResults res = di.decompileFunction(f, 300, monitor);
                String styleUsed = "decompile";

                // 2. If timed out, try "firstpass"
                if (res == null || !res.decompileCompleted() || res.getDecompiledFunction() == null) {
                    println("  Full decompile timed out or failed (" + (res == null ? "null" : res.getErrorMessage()) + "), retrying with 'firstpass'...");
                    di.setSimplificationStyle("firstpass");
                    res = di.decompileFunction(f, 120, monitor);
                    styleUsed = "firstpass";
                }

                // 3. If still failed, try "jumpdisplacement"
                if (res == null || !res.decompileCompleted() || res.getDecompiledFunction() == null) {
                    println("  firstpass failed, retrying with 'jumpdisplacement'...");
                    di.setSimplificationStyle("jumpdisplacement");
                    res = di.decompileFunction(f, 120, monitor);
                    styleUsed = "jumpdisplacement";
                }

                String safeName = name.replaceAll("[^A-Za-z0-9_.-]", "_");
                if (safeName.length() > 180) safeName = safeName.substring(0, 180);
                String outFileName = safeName + "@" + f.getEntryPoint() + ".c";
                File outFile = new File(gmlDir, outFileName);

                if (res != null && res.decompileCompleted() && res.getDecompiledFunction() != null) {
                    try (PrintWriter w = new PrintWriter(new FileWriter(outFile))) {
                        w.println("// " + name + " @ " + f.getEntryPoint());
                        w.println("// Decompiled using Ghidra style: " + styleUsed);
                        w.print(res.getDecompiledFunction().getC());
                    }
                    println("  -> SUCCESS: saved to " + outFileName + " (" + outFile.length() + " bytes)");
                    pwLog.println(addrStr + "\t" + name + "\tSUCCESS\t" + styleUsed + "\t" + outFileName);
                    success++;
                } else {
                    println("  -> FAILED completely: " + (res == null ? "null" : res.getErrorMessage()));
                    pwLog.println(addrStr + "\t" + name + "\tFAILED\t" + (res == null ? "null" : res.getErrorMessage()) + "\t-");
                    stillFailed++;
                }
                pwLog.flush();
            }
        }

        di.dispose();
        println(String.format("FINISHED RETRY: %d succeeded, %d failed out of %d.", success, stillFailed, targets.size()));
    }
}
