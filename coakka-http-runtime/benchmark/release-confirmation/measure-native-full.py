#!/usr/bin/env python3
"""Run the Native C release-confirmation fixture with terminal accounting."""

from campaign import main


if __name__ == "__main__":
    main(["--fixtures", "native", "--output", "full-retest-runs"])
