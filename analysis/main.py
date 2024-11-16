import subprocess

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

src_path = '/home/edwin/cs6600/cs6600_ooosim/src/'

def run_bp_sim(N, S, trace):
    command = "./ooosim "
    command += f"{N} {S} ./../unittest/test/{trace}"
    result = subprocess.run(command, shell=True, capture_output=True, text=True, cwd=src_path)

    lines = result.stdout.split('\n')
    return float(lines[-2].replace('IPC', '').replace('=', '').strip()),

def generate_plot(trace, name):
    S = [8, 16, 32, 64, 128, 256]
    N = [2, 4, 8]

    for n in N:
        ipc = []
        for s in S:
            out  = run_bp_sim(n, s, trace)
            ipc.append(out)
        plt.plot(S, ipc, '-o')

    plt.xlabel('Scheduling Queue Size (S)')
    plt.ylabel('IPC')
    plt.title(f'IPC vs S for various N for {trace}')
    plt.legend(N, title="Fetch Bandwidth (N)")
    plt.grid(True)
    plt.savefig(name)
    plt.close()

if __name__ == '__main__':
    generate_plot('gcc_trace.txt', 'plot1.png')
    generate_plot('perl_trace.txt', 'plot2.png')



    
    
    