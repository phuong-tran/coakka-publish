#!/usr/bin/env python3
"""Describe the six installed-package fixtures for the release confirmation."""

import argparse
import json
import os
import shutil
import sys
from pathlib import Path
import signal
import subprocess
import time

import replay


def lane(root, name, argv, files, native, env, port=18430, warmup=5):
    def identity(paths):
        return {str(path.resolve(strict=True)): replay.sha(path) for path in paths}

    argv[0] = str(Path(argv[0]).absolute())
    return {"campaign": "confirmation", "name": name, "coakka": name,
            "port": port, "warmup": warmup, "argv": argv,
            "exe_sha256": replay.sha(argv[0]), "files": identity(files),
            "native": identity(native), "env": env, "cwd": str(root)}


def fixtures(root):
    core = root / "sdk/lib/libcoakka_http_host.so.2.2.0"
    jvm = root / "jvm/native"
    jar = root / "inputs/coakka-http-runtime-jvm-2.2.1.jar"
    stdlib = root / "inputs/kotlin-stdlib-2.3.10.jar"
    python = root / "python/bin/python"
    py_package = root / "python/lib/python3.13/site-packages/coakka_http"
    go = root / "go-fixed"
    default = {"COAKKA_BENCH_CPU_POLICY": "auto", "GOMAXPROCS": "2"}
    def executable(name):
        path = shutil.which(os.environ.get(f"BENCHMARK_{name.upper()}", name))
        if not path:
            raise RuntimeError(f"Missing {name}. Set BENCHMARK_{name.upper()} to its executable.")
        return Path(path).resolve(strict=True)

    node, bun, java = (executable(name) for name in ("node", "bun", "java"))
    for executable, expected in {
        node: "39d79a0ac7e784fac9898a9a6b30be03ab1b3ae317e0ba094341c237c5ab6431",
        bun: "616f267a34278ff5ac282df37ffdfba1d7141f4f6926bca99af2cd6ef3ad32b1",
        python: "273cb29e073b748379ba546affa562283ed795f84ff230349e12ce610e60975c",
        java: "2cb856302ac72921bdf09f7e56ccc994a03161e1db94bd8064233a7d5a9bf992",
    }.items():
        assert replay.sha(executable) == expected, executable
    out = {}
    out["native"] = lane(root, "native",
                         [str(root / "native-fixed"), "18431", "full", "32", "64", "2"],
                         [], [core], {}, port=18431)
    out["python"] = lane(root, "python", [str(python), "-I", "-u", str(root / "server.py")],
                         [root / "server.py", *py_package.glob("*.py")],
                         list((py_package / "native").glob("*.so*")), default)
    for name, engine in (("node", node), ("bun", bun)):
        script = root / name / "server.mjs"
        package = script.parent / "node_modules" / f"coakka-http-runtime-{name}"
        platform = package / "platforms/linux-arm64"
        out[name] = lane(root, name, [str(engine), str(script)],
                         [script, package / "src/index.mjs", platform / "src/index.mjs"],
                         list((platform / "native").glob("*")), default)
    out["go"] = lane(root, "go", [str(go), "coakka", "18430"], [go], [core], default)
    out["jvm"] = lane(root, "jvm",
                      [str(java), "-XX:ActiveProcessorCount=2",
                       f"-Dcoakka.http.host.path={jvm / 'libcoakka_http_host.so'}",
                       f"-Dcoakka.http.bridge.path={jvm / 'libcoakka_http_jvm.so'}",
                       "-cp", f"{root / 'classes'}:{jar}:{stdlib}", "FixedServer"],
                      [jar, root / "classes/FixedServer.class"],
                      [jvm / "libcoakka_http_host.so", jvm / "libcoakka_http_jvm.so"],
                      default, warmup=30)
    return out


def check(entries):
    out = Path("preflight")
    out.mkdir(exist_ok=True)
    for name, item in entries.items():
        log_path = out / f"{name}.log"
        with log_path.open("w") as log:
            server = subprocess.Popen(["taskset", "-c", "0-1", *item["argv"]],
                                      env=item["env"], stdout=log, stderr=subprocess.STDOUT)
            try:
                for _ in range(120):
                    assert server.poll() is None, f"{name} exited before ready"
                    try:
                        replay.probe(item["port"])
                        if name == "native" or "coakka-runtime-info=" in log_path.read_text():
                            break
                    except OSError:
                        pass
                    time.sleep(0.15)
                else:
                    raise RuntimeError(f"{name} readiness timeout")
                loaded = replay.running_identity(server, item, log_path)
                replay.write_json(out / f"{name}.json", loaded)
                print(f"{name} loaded, configured and serving HTTP/1.1", flush=True)
            finally:
                if server.poll() is None:
                    server.send_signal(signal.SIGTERM)
                    server.wait(timeout=15)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="verify startup and identity without timed load")
    parser.add_argument("--fixtures", nargs="+", choices=("native", "python", "node", "bun", "go", "jvm"))
    parser.add_argument("--output", type=Path, default=Path("results-confirmation"))
    args = parser.parse_args(argv)
    entries = fixtures(Path.cwd())
    if args.fixtures:
        entries = {name: entries[name] for name in args.fixtures}
    if args.check:
        check(entries)
        return
    profile = ("python", "node", "bun", "go", "jvm")
    schedule = [{**entries["native"], "round": n} for n in range(1, 4) if "native" in entries]
    for n in range(1, 4):
        order = profile[::-1] if n == 2 else profile
        schedule.extend({**entries[name], "round": n} for name in order if name in entries)
    schedule_path = args.output.with_suffix(".schedule.json")
    replay.write_json(schedule_path, schedule)
    subprocess.run([sys.executable, "-B", str(Path(__file__).with_name("replay.py")),
                    str(schedule_path), str(args.output)], check=True)
    admitted = sum(
        json.loads(path.read_text())["admitted"]
        for path in args.output.glob("*/receipt.json")
    )
    if admitted != len(schedule):
        raise SystemExit(f"Only {admitted} of {len(schedule)} runs admitted; see {args.output}.")


if __name__ == "__main__":
    main()
