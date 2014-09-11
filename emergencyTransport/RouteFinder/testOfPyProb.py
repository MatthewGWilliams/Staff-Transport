import PyProb
myProb = PyProb.PyProblem(8)
myProb.insert_time(1,2,0.003)
print myProb.get_time(2, 1)

myProb.insert_Vehicle(1,1,1,1, 100, 700)
print myProb.insert_Request(1, True, -1, 580, 8, 420, [[1,0]], [[2,0]])
myProb.setup_solution()
fish = myProb.returnSolution()
