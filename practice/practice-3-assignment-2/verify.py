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

for k in [1,2,4]:
    out=run(4,f'{k}\n',flags=['--mode','private'])
    private=out.split('Private:\n')[1]
    assert sorted(map(int,re.findall(r'I am (\d+) thread',private)))==list(range(k))
for n in [0,1,5,10000]:
    for k in [2,4,8]:
        out=run(5,f'{k} {n}\n',flags=['--extra'])
        parts=list(map(int,re.findall(r'\[\d+\]: Sum = (\d+)',out)))
        total=int(re.search(r'^Sum = (\d+)$',out,re.M)[1])
        assert len(parts)==k and sum(parts)==total==n*(n+1)//2
    out=run(5,f'{n}\n')
    assert int(re.search(r'^Sum = (\d+)$',out,re.M)[1])==n*(n+1)//2
run(5,'0 5\n',flags=['--extra'],fail=True)
run(5,'4294967296\n',fail=True)

print("OK: практика 2,", checks, "запусков")
