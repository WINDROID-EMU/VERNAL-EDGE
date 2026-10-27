// Decompile the 26 remaining complex/huge functions.
// Tries full C decompile -> normalize -> formatted disassembly fallback.
//@category YYC
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import java.io.*;
import java.util.*;

public class DecompilePending extends GhidraScript {

    @Override
    public void run() throws Exception {
        File baseDir = new File("exe_decompiled");
        File pendingList = new File(baseDir, "pending_26.txt");
        File gmlDir = new File(baseDir, "gml");
        gmlDir.mkdirs();

        if (!pendingList.exists()) {
            println("pending_26.txt not found!");
            return;
        }

        List<String[]> targets = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(pendingList))) {
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

        println("Found " + targets.size() + " pending functions to process.");

        DecompInterface di = new DecompInterface();
        DecompileOptions options = new DecompileOptions();
        options.setMaxPayloadMBytes(256);
        di.setOptions(options);
        di.toggleCCode(true);
        di.toggleSyntaxTree(false);
        di.setSimplificationStyle("decompile");
        di.openProgram(currentProgram);

        AddressFactory af = currentProgram.getAddressFactory();
        FunctionManager fm = currentProgram.getFunctionManager();
        Listing listing = currentProgram.getListing();

        File indexFile = new File(baseDir, "index.tsv");

        for (int i = 0; i < targets.size(); i++) {
            String addrStr = targets.get(i)[0];
            String name = targets.get(i)[1];
            println(String.format("[%d/%d] Processing %s (%s)...", (i + 1), targets.size(), name, addrStr));

            Address addr = af.getAddress(addrStr);
            if (addr == null) {
                println("  ERROR: could not parse address " + addrStr);
                continue;
            }

            Function f = fm.getFunctionAt(addr);
            if (f == null) {
                f = fm.getFunctionContaining(addr);
            }
            if (f == null) {
                println("  ERROR: function not found at " + addr);
                continue;
            }

            String safeName = name.replaceAll("[^A-Za-z0-9_.-]", "_");
            if (safeName.length() > 180) safeName = safeName.substring(0, 180);
            String outFileName = safeName + "@" + f.getEntryPoint() + ".c";
            File outFile = new File(gmlDir, outFileName);

            // 1. Try full "decompile" with 180s timeout
            di.setSimplificationStyle("decompile");
            DecompileResults res = di.decompileFunction(f, 180, monitor);
            String styleUsed = "decompile";

            // 2. Fallback to "normalize" if needed
            if (res == null || !res.decompileCompleted() || res.getDecompiledFunction() == null) {
                println("  Full decompile timed out or failed, retrying with 'normalize'...");
                di.setSimplificationStyle("normalize");
                res = di.decompileFunction(f, 90, monitor);
                styleUsed = "normalize";
            }

            if (res != null && res.decompileCompleted() && res.getDecompiledFunction() != null) {
                try (PrintWriter w = new PrintWriter(new FileWriter(outFile))) {
                    w.println("// " + name + " @ " + f.getEntryPoint());
                    w.println("// Decompiled using Ghidra style: " + styleUsed);
                    w.print(res.getDecompiledFunction().getC());
                }
                println("  -> SUCCESS (C): " + outFileName + " (" + outFile.length() + " bytes)");
            } else {
                // 3. Fallback: Dump clean annotated assembly/disassembly
                println("  -> Decompiler exceeded limit, dumping disassembly fallback...");
                styleUsed = "disassembly-fallback";
                try (PrintWriter w = new PrintWriter(new FileWriter(outFile))) {
                    w.println("// " + name + " @ " + f.getEntryPoint());
                    w.println("// Fallback: Formatted native disassembly (function exceeds Ghidra AST decompiler payload)");
                    w.println("/*");
                    InstructionIterator instIter = listing.getInstructions(f.getBody(), true);
                    while (instIter.hasNext()) {
                        Instruction inst = instIter.next();
                        w.println(String.format("  0x%s: %s", inst.getAddress(), inst.toString()));
                    }
                    w.println("*/");
                }
                println("  -> SUCCESS (ASM): " + outFileName + " (" + outFile.length() + " bytes)");
            }

            // Append to index.tsv if not present
            try (PrintWriter pwIndex = new PrintWriter(new FileWriter(indexFile, true))) {
                pwIndex.println(f.getEntryPoint() + "\tgml\t" + name);
            }
        }

        di.dispose();
        println("ALL PENDING FUNCTIONS COMPLETED!");
    }
}
