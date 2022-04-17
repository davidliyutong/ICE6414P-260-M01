import sys
import numpy as np
import time

s_mat_m_path, s_mat_n_path, s_resul_path = sys.argv[1:4]
M, N = np.loadtxt('M.csv',delimiter=','), np.loadtxt('N.csv',delimiter=',')
start_t = time.time()
result = np.matmul(M, N)
end_t = time.time() - start_t
print(f"Time elapsed: {end_t}")