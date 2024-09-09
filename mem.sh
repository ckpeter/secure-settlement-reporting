#!/bin/bash

# Validate if a command to run was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <command> [command_arguments]"
    exit 1
fi

# Run the command in the background and capture its PID
"$@" &
PID=$!

# Create log filename with a "log_" prefix based on the PID
LOG_FILE="log_${PID}.csv"

# Logging basic information
echo "Logging to $LOG_FILE for command: $@ (PID: $PID)"

# Write the program command and headers as the first lines in the log file
echo "Command: $@" > $LOG_FILE
echo "Timestamp, CPU(%), Swap(MB), RAM(MB)" >> $LOG_FILE

# Save the initial network statistics for later comparison
INITIAL_TRAFFIC=$(ifconfig | grep bytes | grep -v "(0.0 B)")

# Interval for memory logging
LOG_INTERVAL=60
SECONDS_ELAPSED=0
TOTAL_SECONDS=0

# Variables to track memory usage peaks
HIGH_WATER_MARK=0
HIGH_WATER_TIMESTAMP=""
HIGH_WATER_SECONDS=0

# Logging memory usage function
log_memory_usage() {
    CURRENT_MEMORY=$(ps -p $PID -o %cpu,rss,vsz --no-headers | awk -v date="$(date)" '{print date "," $1 "," $3/1024 "," $2/1024}')
    RAM_MB=$(echo $CURRENT_MEMORY | awk -F, '{print $4}')
    if (( $(echo "$RAM_MB > $HIGH_WATER_MARK" | bc -l) )); then
        HIGH_WATER_MARK=$RAM_MB
        HIGH_WATER_TIMESTAMP=$(date)
        HIGH_WATER_SECONDS=$TOTAL_SECONDS
    fi
    echo $CURRENT_MEMORY >> $LOG_FILE
}

# Initial log entry
log_memory_usage

# Process monitoring loop
while kill -0 $PID 2>/dev/null; do
    sleep 1
    SECONDS_ELAPSED=$((SECONDS_ELAPSED + 1))
    TOTAL_SECONDS=$((TOTAL_SECONDS + 1))
    if [ $SECONDS_ELAPSED -ge $LOG_INTERVAL ]; then
        log_memory_usage
        SECONDS_ELAPSED=0
    fi
done

# Time conversion for peak usage reporting
HOURS=$(($HIGH_WATER_SECONDS / 3600))
MINUTES=$((($HIGH_WATER_SECONDS % 3600) / 60))
TIME_INTO_RUN="${HOURS}h:${MINUTES}m"

# Capture final network statistics at the end of the program run
FINAL_TRAFFIC=$(ifconfig | grep bytes | grep -v "(0.0 B)")

# Report both initial and final network stats
# echo "Network Traffic Comparison:" >> $LOG_FILE
echo "Initial Network Traffic:" >> $LOG_FILE
echo "$INITIAL_TRAFFIC" >> $LOG_FILE
echo "Final Network Traffic:" >> $LOG_FILE
echo "$FINAL_TRAFFIC" >> $LOG_FILE

# Also echo these to the terminal
# echo "Network Traffic Comparison:"
echo "Initial Network Traffic:"
echo "$INITIAL_TRAFFIC"
echo "Final Network Traffic:"
echo "$FINAL_TRAFFIC"

# Reporting high water mark for memory usage
echo "Peak RAM: ${HIGH_WATER_MARK} MB at $HIGH_WATER_TIMESTAMP ($TIME_INTO_RUN from start)"