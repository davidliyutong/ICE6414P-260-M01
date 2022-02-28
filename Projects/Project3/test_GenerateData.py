import numpy as np
import pandas
import sys

if __name__ == '__main__':
    problem_size = int(sys.argv[1]) if len(sys.argv) > 1 else 1000
    M = np.random.randn(problem_size,problem_size)
    N = np.random.randn(problem_size,problem_size)
    np.savetxt('N.csv',N, delimiter=',')
    np.savetxt('M.csv',M, delimiter=',')
    