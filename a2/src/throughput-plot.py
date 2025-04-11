#!/usr/bin/env python3
import argparse
from scapy.all import rdpcap, UDP
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict


def calculate_throughput(pcap_file, dst_ip, window_size=1.0):
    """
    Calculate throughput for UDP packets with a specific destination IP.

    Args:
        pcap_file: Path to the PCAP file
        dst_ip: Destination IP to filter
        window_size: Time window in seconds for throughput calculation

    Returns:
        Two lists containing time points and corresponding throughput values
    """
    # Read the PCAP file
    packets = rdpcap(pcap_file)

    # Filter UDP packets with the specified destination IP
    filtered_packets = [pkt for pkt in packets if UDP in pkt and pkt.dst == dst_ip]

    if not filtered_packets:
        print(f"No UDP packets found with destination IP {dst_ip} in {pcap_file}")
        return [], []

    # Get the start time of the capture
    start_time = filtered_packets[0].time

    # Dictionary to store bytes per time window
    bytes_per_window = defaultdict(int)

    # Process each packet
    for pkt in filtered_packets:
        # Calculate relative time from the start
        rel_time = pkt.time - start_time
        # Determine which time window this packet belongs to
        window_index = int(rel_time / window_size)
        # Add packet length to the corresponding window
        bytes_per_window[window_index] += len(pkt)

    # Convert to throughput (bits per second)
    time_points = []
    throughput = []

    for window_idx in sorted(bytes_per_window.keys()):
        # Center of the time window
        time_point = (window_idx + 0.5) * window_size
        # Convert bytes to bits and divide by window size to get bps
        tput = (bytes_per_window[window_idx] * 8) / window_size

        time_points.append(time_point)
        throughput.append(tput)

    return time_points, throughput


def main():
    parser = argparse.ArgumentParser(description="Analyze throughput from PCAP files")
    parser.add_argument("pcap1", help="First PCAP file")
    parser.add_argument("pcap2", help="Second PCAP file")
    parser.add_argument(
        "--dst1",
        default="10.1.1.1",
        help="Destination IP for first PCAP (default: 10.1.1.1)",
    )
    parser.add_argument(
        "--dst2",
        default="10.1.1.2",
        help="Destination IP for second PCAP (default: 10.1.1.2)",
    )
    parser.add_argument(
        "--window",
        type=float,
        default=1.0,
        help="Time window in seconds for throughput calculation (default: 1.0)",
    )
    parser.add_argument(
        "--output",
        default="throughput.png",
        help="Output file name (default: throughput.png)",
    )

    args = parser.parse_args()

    # Calculate throughput for both PCAP files
    time1, tput1 = calculate_throughput(args.pcap1, args.dst1, args.window)
    time2, tput2 = calculate_throughput(args.pcap2, args.dst2, args.window)

    # Create the plot
    plt.figure(figsize=(10, 6))

    if time1 and tput1:
        plt.plot(
            time1, np.array(tput1) / 1_000_000, label=f"AP1 ({args.dst1})", linewidth=2
        )

    if time2 and tput2:
        plt.plot(
            time2, np.array(tput2) / 1_000_000, label=f"AP2 ({args.dst2})", linewidth=2
        )

    plt.xlabel("Time (seconds)")
    plt.ylabel("Throughput (Mbps)")
    plt.title("UDP Throughput over Time")
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.legend()

    # Save the plot
    plt.savefig(args.output, dpi=300, bbox_inches="tight")
    print(f"Plot saved as {args.output}")

    # Also display the plot
    plt.show()


if __name__ == "__main__":
    main()
