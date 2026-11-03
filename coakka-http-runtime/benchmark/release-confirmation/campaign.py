"""Run three connector rounds and resume from the accepted-lane ledger."""

import json
import subprocess
import sys
from pathlib import Path

# Freeze one response profile per connector before collecting timed samples.
profiles = [
    ("python", "shared"),
    ("node", "shared"),
    ("bun", "shared"),
    ("go", "fresh"),
    ("jvm", "fresh"),
]
report = Path("campaign.json")
accepted = json.loads(report.read_text()) if report.exists() else {}
for round_number in range(1, 4):
    order = profiles if round_number != 2 else reversed(profiles)
    for host, mode in order:
        lane = f"{round_number}-{host}-{mode}"
        if lane in accepted:
            continue
        for attempt in range(1, 5):
            name = f"{lane}-attempt{attempt}"
            folder = Path("attempts") / name
            # Preserve earlier attempt logs when resuming a campaign.
            if folder.exists():
                continue
            result = subprocess.run(
                [sys.executable, "run-public.py", lane, name],
                capture_output=True,
                text=True,
                timeout=250,
            )
            log = result.stdout + result.stderr
            (folder / "runner.log").write_text(log)
            state = json.loads((folder / "state.json").read_text())
            assert state["governors_restored"], name
            if result.returncode == 0:
                assert state["complete"], name
                accepted[lane] = name
                report.write_text(json.dumps(accepted, indent=2) + "\n")
                print(result.stdout.strip(), flush=True)
                break
            print(name, log.splitlines()[-1], flush=True)
            if not any(
                reason in log
                for reason in ("warmup thermal rejection", "thermal admission timeout")
            ):
                raise RuntimeError(f"Unexpected benchmark failure: {name}")
        else:
            raise RuntimeError(f"No admitted run for {lane}")
assert len(accepted) == 15
