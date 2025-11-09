# MM1 Queue Implementation

This project implements an MM1 queue simulation in Python. An MM1 queue is a single-server queue where arrivals follow a Poisson process and service times are exponentially distributed.

## Overview

The `MM1Queue` class provides methods to manage queue operations and calculate performance metrics such as wait times and queue lengths.

## Installation

To install the required dependencies, run:

```
pip install -r requirements.txt
```

## Usage

Here is a simple example of how to use the `MM1Queue` class:

```python
from src.mm1_queue import MM1Queue

# Create an instance of MM1Queue
queue = MM1Queue(arrival_rate=1.0, service_rate=1.5)

# Enqueue a customer
queue.enqueue()

# Dequeue a customer
queue.dequeue()

# Get the current queue length
length = queue.get_queue_length()

# Calculate the average wait time
wait_time = queue.calculate_wait_time()
```

## Running Tests

To run the tests for this project, you can use:

```
pytest tests/test_mm1_queue.py
```

## License

This project is licensed under the MIT License.