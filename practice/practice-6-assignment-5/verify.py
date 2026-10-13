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
    out=run(13,processes=p)
    pairs=sorted((int(a),int(b)) for a,b in re.findall(r"\[(\d+)\]: receive message '(\d+)'",out))
    assert pairs==[(i, (p-1 if i==0 else i-1)) for i in range(p)]
    out=run(14,processes=p)
    assert sorted((int(a),int(b)) for a,b in re.findall(r"receive message '(\d+)' from (\d+)",out))==[(i,i) for i in range(1,p)]
    out=run(15,processes=p)
    assert sorted((int(a),int(b)) for a,b in re.findall(r"\[(\d+)\]: receive message '(\d+)'",out))==[(i,(i-1)%p) for i in range(p)]
    out=run(16,processes=p)
    triples=sorted(tuple(map(int,t)) for t in re.findall(r"\[(\d+)\]: receive message '(\d+)' from (\d+)",out))
    assert triples==[(i,j,j) for i in range(p) for j in range(p) if i!=j]

print("OK: практика 5,", checks, "запусков")
