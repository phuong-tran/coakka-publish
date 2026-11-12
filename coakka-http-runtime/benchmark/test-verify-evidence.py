"""Regression checks for the public benchmark's admission and table verification."""

import copy
import importlib.util
from pathlib import Path
import tempfile
import unittest


SOURCE = Path(__file__).with_name("verify-evidence.py")
SPEC = importlib.util.spec_from_file_location("benchmark_evidence", SOURCE)
evidence = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(evidence)


class EvidenceChecks(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.row = next(row for row in evidence.load_evidence("confirmation") if row["admitted"])

    def test_reject_admitted_run_at_temperature_limit(self):
        row = copy.deepcopy(self.row)
        row["thermal"].append({"phase": "timed", "temp_mC": 50000,
                               "throttled": "throttled=0x0", "fan_state": 4})
        with self.assertRaisesRegex(ValueError, "thermal admission"):
            evidence.verify_run(row, "confirmation")

    def test_reject_latency_column_that_disagrees_with_samples(self):
        row = copy.deepcopy(self.row)
        row["metrics"]["p99_ms"] += 0.1
        with self.assertRaisesRegex(ValueError, "p99 drift"):
            evidence.verify_run(row, "confirmation")

    def test_reject_wrong_load_connection_count(self):
        row = copy.deepcopy(self.row)
        row["timed_argv"][6] = "32"
        with self.assertRaisesRegex(ValueError, "timed load command"):
            evidence.verify_run(row, "confirmation")

    def test_reject_hand_edited_table(self):
        with tempfile.TemporaryDirectory() as root:
            page = Path(root) / "README.md"
            page.write_text("<!-- summary:start -->\nold value\n<!-- summary:end -->\n")
            with self.assertRaisesRegex(ValueError, "table drift"):
                evidence.update_block(page, "summary", "current value", False)


if __name__ == "__main__":
    unittest.main()
