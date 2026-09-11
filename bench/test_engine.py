import subprocess
import sys
import tempfile
from pathlib import Path

from bench.selfplay import EngineConfig, UciEngine


def main():
    root = Path(__file__).resolve().parent.parent
    binary = str(Path(sys.argv[1]).resolve()) if len(sys.argv) > 1 else str(root / "build/src/messier")
    with tempfile.TemporaryDirectory() as directory:
        check = str(Path(directory) / "check")
        subprocess.run(
            ["g++", "-std=c++20", "-I", str(root / "include"), "-x", "c++", "-",
             str(root / "src/movegen/types.cpp"), "-o", check],
            input='''
#include <cassert>
#include <sstream>
#include <messier/search/tt.hpp>
int main() {
    TTable table(1);
    uint64_t hash = 0x1234567800000000ULL;
    assert(!std::get<0>(table.probe(hash)));
    table.push(hash, Transposition(FLAG_EXACT, hash, 1, 42, 42, Move::none(), 0));
    auto [hit, entry] = table.probe(hash);
    assert(hit && entry->score == 42);
    table.clear();
    assert(!std::get<0>(table.probe(hash)));
    for (Move m : {Move::none(), Move(e5, d6, EN_PASSANT), Move(a7, a8, PR_QUEEN)}) {
        std::ostringstream output;
        output << m;
        assert(output.str() == m.to_string());
    }
    assert(Move::none().to_string() == "0000");
    assert(Move(e5, d6, EN_PASSANT).to_string() == "e5d6");
    assert(Move(a7, a8, PR_QUEEN).to_string() == "a7a8q");
    for (const char* text : {"", "e2", "i2e4", "e0e4", "e7e8x", "e7e8qq"})
        assert(Move::from_string(text) == Move::none());
}
''', text=True, check=True, timeout=60,
        )
        subprocess.run([check], check=True, timeout=5)

    config = EngineConfig("messier", (binary,), None, ())
    with UciEngine(config, 5) as engine:
        engine.send(" \tposition  startpos\t moves  e2e4 \t e7e5  ")
        engine.send("printpos")
        fen = engine.wait_for(lambda line: line.startswith("FEN:"), 5, "position")
        assert fen == "FEN: rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2", fen
        for command in [" ", "position", "position fen", "go depth", "go nodes x",
                        "go wtime 999999999999999999999", "go searchmoves", "bench nodes",
                        "setoption name Hash value 0", "setoption name Hash value x", "eval", "probe"]:
            engine.send(command)
            engine.sync(5)
        engine.send("printpos")
        assert engine.wait_for(lambda line: line.startswith("FEN:"), 5, "position") == fen

        for fen, move, score in [
            ("7k/6Q1/6K1/8/8/8/8/8 b - - 0 1", "0000", "mate 0"),
            ("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1", "0000", "cp 0"),
            ("7k/5Q2/6K1/8/8/8/8/8 w - - 0 1", "f7e8", "mate 1"),
            ("k7/8/8/3pP3/8/8/8/7K w - d6 0 1", "e5d6", "cp 171"),
        ]:
            engine.new_game(5)
            engine.send(f"position fen {fen}")
            engine.send("go depth 1")
            info = engine.wait_for(lambda line: line.startswith("info depth 1 "), 5, "score")
            assert f" score {score} " in info, info
            assert engine.wait_for(lambda line: line.startswith("bestmove "), 5, "move") == f"bestmove {move}"

        engine.send("stop")
        engine.send("bench 1")
        nodes = engine.wait_for(lambda line: line.startswith("Nodes searched"), 10, "benchmark")
        assert int(nodes.split(":")[1]) > 0, nodes
        engine.sync(5)
        engine.send("position fen 7k/6Q1/6K1/8/8/8/8/8 b - - 0 1")
        engine.send("go infinite")
        engine.wait_for(lambda line: line.startswith("info depth 30 "), 5, "maximum depth")
        engine.send("isready")
        assert engine.wait_for(lambda line: line == "readyok" or line.startswith("bestmove "), 5, "ready") == "readyok"
        engine.send("stop")
        assert engine.wait_for(lambda line: line.startswith("bestmove "), 5, "stopped move") == "bestmove 0000"
        engine.send("go infinite")
        engine.wait_for(lambda line: line.startswith("info depth 30 "), 5, "maximum depth")
        engine.send("quit")
        assert engine.process.wait(timeout=5) == 0
    print("Engine regression checks passed")


if __name__ == "__main__":
    main()
