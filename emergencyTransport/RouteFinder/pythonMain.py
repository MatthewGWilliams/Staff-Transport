import csv
from decimal import Decimal

temp = open('C:/Users/Matthew/Dropbox/Cardiff MSc/Creating test sets/SaturdayAMTimes.csv', 'rb')
times = csv.reader(temp)
times = [[int(row[0]), int(row[1]), float(row[2])] for row in times]
temp.close()

temp = open('C:/Users/Matthew/Dropbox/Cardiff MSc/Creating test sets/SaturdayAMRequestsFormatted.csv', 'rb')
requests = csv.reader(temp)
requests = [[int(row[0]), int(row[1]), int(row[2]), float(row[3]), float(row[4]), int(row[5]), bool(int(row[6]))] for row in requests]
temp.close

import PyProb

myProb = PyProb.PyProblem(5)
for time in times:
    myProb.insert_time(time[0], time[1], time[2])

counter = 0
for request in requests:
    counter += 1
    myProb.insert_Request(counter, request[6], request[2], request[3], request[5], request[4], [[request[0], 0]], [[request[1], 0]])

for i in range(1,5):
    myProb.insert_Vehicle(i, 1, 1, 4, float(60), float(780))

myProb.insert_Vehicle(5, 1, 1, 7, float(60), float(780))

myProb.setup_solution()
print myProb.returnSolution()
    
