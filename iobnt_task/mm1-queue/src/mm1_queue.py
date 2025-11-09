import os
import random
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

class MM1Queue:
    """
    Simulates an M/M/1 queue system with exponential inter-arrival and service times.
    """

    def __init__(self, inter_arrival_time, service_time, seed=None):
        """
        Initializes the MM1Queue.

        Args:
            inter_arrival_time (float): Mean inter-arrival time (X = 1/lambda).
            service_time (float): Mean service time (Y = 1/mu).
            seed (int, optional): Random seed for reproducibility.
        """
        self.inter_arrival_time = inter_arrival_time
        self.service_time = service_time
        self.seed = seed
        self.reset()

    def reset(self):
        """
        Resets the queue and metrics for a new simulation run.
        """
        self.queue = []
        self.current_time = 0.0
        self.waiting_times = []
        self.inter_arrival_times = []
        self.service_times = []
        self.queue_lengths = []
        self.arrival_times = []
        if self.seed is not None:
            random.seed(self.seed)
            np.random.seed(self.seed)

    def simulate(self, total_time=6000.0):
        """
        Runs the simulation for the specified total_time.

        Args:
            total_time (float): Total simulation time in seconds.
        """
        self.reset()
        next_arrival = np.random.exponential(self.inter_arrival_time)
        next_departure = float('inf')
        num_in_queue = 0
        last_event_time = 0.0

        while self.current_time < total_time:
            if next_arrival < next_departure:
                # Arrival event
                self.current_time = next_arrival
                self.arrival_times.append(self.current_time)
                self.inter_arrival_times.append(self.current_time - last_event_time)
                last_event_time = self.current_time
                num_in_queue += 1
                self.queue_lengths.append((self.current_time, num_in_queue))
                if num_in_queue == 1:
                    # Start service immediately
                    service_time = np.random.exponential(self.service_time)
                    self.service_times.append(service_time)
                    next_departure = self.current_time + service_time
                    self.waiting_times.append(0.0)
                else:
                    self.waiting_times.append(None)  # Will be filled at departure
                next_arrival = self.current_time + np.random.exponential(self.inter_arrival_time)
            else:
                # Departure event
                self.current_time = next_departure
                num_in_queue -= 1
                self.queue_lengths.append((self.current_time, num_in_queue))
                # Find the first customer with None waiting time and fill it
                for i, wt in enumerate(self.waiting_times):
                    if wt is None:
                        self.waiting_times[i] = self.current_time - self.arrival_times[i]
                        break
                if num_in_queue > 0:
                    service_time = np.random.exponential(self.service_time)
                    self.service_times.append(service_time)
                    next_departure = self.current_time + service_time
                else:
                    next_departure = float('inf')

        # Remove None values from waiting_times (if any)
        self.waiting_times = [wt for wt in self.waiting_times if wt is not None]
        self.inter_arrival_times = self.inter_arrival_times[1:]  # first inter-arrival time is artificial (from 0), removed for statistics.

    def get_queue_timeline(self, duration=60.0):
        """
        Returns the queue length timeline for the first `duration` seconds.

        Args:
            duration (float): Duration in seconds.

        Returns:
            tuple: (times, queue_lengths)
        """
        times = []
        lengths = []
        for t, l in self.queue_lengths:
            if t > duration:
                break
            times.append(t)
            lengths.append(l)
        return times, lengths


# ---- Simulation and Plotting ----


def run_all_simulations():
    service_time = 1.0
    inter_arrival_times = [1.1, 1.5, 2.0]
    seeds = [42, 100, 500]
    total_time = 6000.0

    all_waiting = []
    all_interarrival = []
    all_service = []
    all_labels = []
    timelines = {}

    for X, seed in zip(inter_arrival_times, seeds):
        queue = MM1Queue(inter_arrival_time=X, service_time=service_time, seed=seed)
        queue.simulate(total_time=total_time)
        all_waiting.append(queue.waiting_times)
        all_interarrival.append(queue.inter_arrival_times)
        all_service.append(queue.service_times)
        all_labels.append(f"X={X}")
        t, l = queue.get_queue_timeline(duration=60.0)
        timelines[X] = (t, l)

    # Box plots
    fig, axes = plt.subplots(1, 3, figsize=(18, 5))
    sns.boxplot(data=all_waiting, ax=axes[0])
    axes[0].set_xticklabels(all_labels)
    axes[0].set_title("Waiting Time in Queue")
    axes[0].set_xlabel("Inter Arrival Time X (s)")
    axes[0].set_ylabel("Waiting Time (s)")

    sns.boxplot(data=all_interarrival, ax=axes[1])
    axes[1].set_xticklabels(all_labels)
    axes[1].set_title("Inter Arrival Time")
    axes[1].set_xlabel("Inter Arrival Time X (s)")
    axes[1].set_ylabel("Inter Arrival Time (s)")

    sns.boxplot(data=all_service, ax=axes[2])
    axes[2].set_xticklabels(all_labels)
    axes[2].set_title("Service Time")
    axes[2].set_xlabel("Inter Arrival Time X (s)")
    axes[2].set_ylabel("Service Time (s)")

    plt.tight_layout()
    plt.show()

    # Timeline plot
    plt.figure(figsize=(10, 6))
    for X in inter_arrival_times:
        t, l = timelines[X]
        plt.step(t, l, where='post', label=f"X={X}", linewidth=1, marker='o', alpha=0.7)
    plt.title("Queue Length Timeline (First 60 Seconds)")
    plt.xlabel("Simulation Time (s)")
    plt.ylabel("Number of Items in Queue")
    plt.legend()
    plt.show()


if __name__ == "__main__":
    run_all_simulations()