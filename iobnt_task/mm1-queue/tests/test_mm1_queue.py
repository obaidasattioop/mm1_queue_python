import unittest
from src.mm1_queue import MM1Queue

class TestMM1Queue(unittest.TestCase):

    def setUp(self):
        self.queue = MM1Queue(arrival_rate=1.0, service_rate=2.0)

    def test_enqueue(self):
        self.queue.enqueue('customer1')
        self.assertEqual(self.queue.get_queue_length(), 1)

    def test_dequeue(self):
        self.queue.enqueue('customer1')
        customer = self.queue.dequeue()
        self.assertEqual(customer, 'customer1')
        self.assertEqual(self.queue.get_queue_length(), 0)

    def test_get_queue_length(self):
        self.queue.enqueue('customer1')
        self.queue.enqueue('customer2')
        self.assertEqual(self.queue.get_queue_length(), 2)

    def test_calculate_wait_time(self):
        self.queue.enqueue('customer1')
        self.queue.enqueue('customer2')
        wait_time = self.queue.calculate_wait_time()
        self.assertGreaterEqual(wait_time, 0)

if __name__ == '__main__':
    unittest.main()