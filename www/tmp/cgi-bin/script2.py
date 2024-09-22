#!/usr/bin/env python3

from datetime import datetime

def main():
    current_date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(f"Hello! The current date and time is: {current_date}")

if __name__ == "__main__":
    main()
