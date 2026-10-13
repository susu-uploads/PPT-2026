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

for k,n in [(1,5),(2,4),(3,2),(4,0),(4,10000)]:
    out=run(6,f'{k} {n}\n')
    parts=list(map(int,re.findall(r'\[\d+\]: Sum = (\d+)',out)))
    assert len(parts)==k and sum(parts)==n*(n+1)//2
    assert int(re.search(r'^Sum = (\d+)$',out,re.M)[1])==n*(n+1)//2
for schedule in ['static','static,1','static,2','dynamic','dynamic,2','guided','guided,2']:
    out=run(6,flags=['--schedule',schedule])
    pairs=[(int(t),int(i)) for t,i in re.findall(r'\[(\d+)\]: calculation of the iteration number (\d+)',out)]
    assert sorted(i for t,i in pairs)==list(range(1,11)) and all(0<=t<4 for t,i in pairs)
    assert re.search(r'^Sum = 55$',out,re.M)
for n in [1,2,3,10]:
    data,expected=matrix_input(n)
    assert list(map(float,run(8,data).split()))==expected
assert list(map(float,run(8,'2\n1 1 4 4\n5 5 3 3\n').split()))==[8,8,32,32]
for k in [1,2,3,4]:
    out=run(9,f'{k}\n')
    assert sorted(map(int,re.findall(r'came in section (\d+)',out)))==[1,2,3]
    assert sorted(map(int,re.findall(r'\[(\d+)\]: parallel region',out)))==list(range(k))
run(8,'11\n',fail=True)
run(8,'2\n1 2\n',fail=True)

print("OK: практика 3,", checks, "запусков")
