// Recover GML names in a GameMaker YYC binary and decompile all functions.
// Usage (headless): -postScript YYCDecompileAll.java <outputDir>
//@category YYC
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.data.StringDataInstance;
import java.io.*;
import java.util.*;

public class YYCDecompileAll extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File outDir = new File(args.length > 0 ? args[0] : "decompiled");
        File gmlDir = new File(outDir, "gml");
        File engDir = new File(outDir, "engine");
        gmlDir.mkdirs();
        engDir.mkdirs();

        int renamed = recoverGmlNames();
        println("Renamed functions from gml_* tables: " + renamed);

        DecompInterface di = new DecompInterface();
        di.toggleCCode(true);
        di.toggleSyntaxTree(false);
        di.setSimplificationStyle("decompile");
        di.openProgram(currentProgram);

        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        int total = currentProgram.getFunctionManager().getFunctionCount();
        int n = 0, ok = 0, fail = 0;
        PrintWriter index = new PrintWriter(new FileWriter(new File(outDir, "index.tsv")));
        PrintWriter failed = new PrintWriter(new FileWriter(new File(outDir, "failed.txt")));
        while (it.hasNext() && !monitor.isCancelled()) {
            Function f = it.next();
            n++;
            if (f.isThunk() || f.isExternal()) continue;
            String name = f.getName();
            boolean isGml = name.startsWith("gml_");
            String safe = name.replaceAll("[^A-Za-z0-9_.-]", "_");
            if (safe.length() > 180) safe = safe.substring(0, 180);
            String fname = safe + "@" + f.getEntryPoint() + ".c";
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            if (r != null && r.decompileCompleted() && r.getDecompiledFunction() != null) {
                File out = new File(isGml ? gmlDir : engDir, fname);
                try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
                    w.println("// " + name + " @ " + f.getEntryPoint());
                    w.print(r.getDecompiledFunction().getC());
                }
                index.println(f.getEntryPoint() + "\t" + (isGml ? "gml" : "engine") + "\t" + name);
                ok++;
            } else {
                failed.println(f.getEntryPoint() + "\t" + name + "\t" + (r == null ? "null" : r.getErrorMessage()));
                fail++;
            }
            if (n % 500 == 0) {
                println(String.format("Progress %d/%d ok=%d fail=%d", n, total, ok, fail));
                index.flush(); failed.flush();
            }
        }
        index.close();
        failed.close();
        di.dispose();
        println(String.format("DONE total=%d ok=%d fail=%d", total, ok, fail));
    }

    /** YYC tables hold {char* name, void* func, ...}. Find pointers to gml_* strings
     *  and rename the function pointed to by the following qword. */
    private int recoverGmlNames() throws Exception {
        Memory mem = currentProgram.getMemory();
        Listing listing = currentProgram.getListing();
        ReferenceManager rm = currentProgram.getReferenceManager();
        FunctionManager fm = currentProgram.getFunctionManager();
        AddressSetView code = currentProgram.getMemory().getExecuteSet();
        int count = 0;
        DataIterator di = listing.getDefinedData(true);
        while (di.hasNext() && !monitor.isCancelled()) {
            Data d = di.next();
            if (!d.hasStringValue()) continue;
            String s = StringDataInstance.getStringDataInstance(d).getStringValue();
            if (s == null || !s.startsWith("gml_")) continue;
            for (Reference ref : rm.getReferencesTo(d.getAddress())) {
                Address from = ref.getFromAddress();
                if (code.contains(from)) continue; // only data tables
                try {
                    long target = mem.getLong(from.add(8));
                    Address ta = from.getNewAddress(target);
                    if (!code.contains(ta)) continue;
                    Function f = fm.getFunctionAt(ta);
                    if (f == null) f = createFunction(ta, null);
                    if (f == null) continue;
                    if (f.getName().startsWith("FUN_") || f.getName().startsWith("thunk_")) {
                        f.setName(s, SourceType.ANALYSIS);
                        count++;
                    }
                } catch (Exception e) { /* not a table entry */ }
            }
        }
        return count;
    }
}
