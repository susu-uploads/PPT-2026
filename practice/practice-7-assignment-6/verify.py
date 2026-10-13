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
    for flags in [[],['--point-to-point']]:
        for word in ['a','abracadabra','a'*100]:
            out=run(17,f'{len(word)}\n{word}\n',flags=flags,processes=p)
            counts={k:int(v) for k,v in re.findall(r'^([a-z]) = (\d+)$',out,re.M)}
            assert counts==dict(collections.Counter(word))
        out=run(18,'1000000\n',flags=flags,processes=p)
        assert abs(float(re.search(r'pi = ([0-9.]+)',out)[1])-math.pi)<1e-8
        # N < P: пустые диапазоны не теряют итерации.
        out=run(18,'1\n',flags=flags,processes=p)
        assert float(re.search(r'pi = ([0-9.]+)',out)[1])==3.2
        for n in [1,2,3,10]:
            data,expected=matrix_input(n)
            out=run(19,data,flags=flags,processes=p)
            assert list(map(float,out.split()))==expected
        out=run(19,'2\n1 3 4 8\n5 4 3 0\n',flags=flags,processes=p)
        assert list(map(float,out.split()))==[14,4,44,16]
        run(17,'3\nrvtabc\n',flags=flags,processes=p,fail=True)
        run(17,'0\na\n',flags=flags,processes=p,fail=True)
        run(17,'1\nA\n',flags=flags,processes=p,fail=True)
        run(18,'0\n',flags=flags,processes=p,fail=True)
        run(19,'11\n',flags=flags,processes=p,fail=True)
        run(19,'2\n1 2\n',flags=flags,processes=p,fail=True)
for task,data in [(17,'3\naba\n'),(18,'10000\n'),(19,'1\n2\n3\n')]:
    for flags in [['--benchmark'],['--benchmark','--point-to-point']]:
        out=run(task,data,flags=flags,processes=2)
        assert float(re.search(r'Time = ([0-9.]+)',out)[1])>0

print("OK: практика 6,", checks, "запусков")
