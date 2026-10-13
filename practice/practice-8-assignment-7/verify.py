"""Проверка реальных запусков упражнений; запускать из каталога практики."""
import collections
import math
import os
from pathlib import Path
import re
import subprocess
import sys

BASE = Path(__file__).resolve().parent
BUILD = Path(sys.argv[1]).resolve() if len(sys.argv)>1 else BASE/'.cache/build'
checks = 0

def run(task, data='', flags=(), processes=None, threads=None, fail=False):
    """Запустить программу с ограничением времени и проверить код завершения."""
    global checks
    command = [str(BUILD/f'task-{task}'), *flags]
    if processes is not None:
        command = ['mpirun', '--bind-to', 'none', '-np', str(processes), *command]
    env = dict(os.environ, OMP_DYNAMIC='FALSE', OMP_NUM_THREADS=str(threads or 4))
    result = subprocess.run(command, input=data, text=True, capture_output=True,
                            timeout=30, env=env)
    assert (result.returncode != 0) if fail else (result.returncode == 0), (command, result.stdout, result.stderr)
    checks += 1
    return result.stdout

def matrix_input(n):
    """Матрица последовательных чисел и единичная матрица дают известный результат."""
    a = list(range(1,n*n+1))
    b = [int(i==j) for i in range(n) for j in range(n)]
    return f'{n}\n'+ ' '.join(map(str,a+b))+'\n', a

for p in [1,2,4]:
    for k in [1,2,4]:
        out=run(31,f'{k}\n',processes=p)
        triples=sorted(tuple(map(int,t)) for t in re.findall(r'I am (\d+) thread from (\d+) process. Number of hybrid threads = (\d+)',out))
        assert triples==sorted((t,r,k*p) for r in range(p) for t in range(k))
        out=run(32,'1000000\n',processes=p,threads=k)
        assert abs(float(re.search(r'pi = ([0-9.]+)',out)[1])-math.pi)<1e-8
for mode in ['serial','openmp']:
    out=run(32,'1000000\n',flags=['--mode',mode])
    assert abs(float(re.search(r'pi = ([0-9.]+)',out)[1])-math.pi)<1e-8
out=run(32,'1\n',processes=4,threads=2)
assert float(re.search(r'pi = ([0-9.]+)',out)[1])==3.2
run(31,'0\n',processes=2,fail=True)
run(32,'0\n',processes=2,fail=True)
run(32,'10\n',flags=['--mode','openmp'],processes=2,fail=True)
run(32,'10\n',flags=['--mode','bad'],processes=2,fail=True)

print("OK: практика 7,", checks, "запусков")
