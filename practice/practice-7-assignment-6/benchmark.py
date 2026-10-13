"""Один прогрев и пять измерений; результат сохраняется в .cache/measurements.json."""
from datetime import datetime
import json
import math
import os
from pathlib import Path
import re
import statistics
import subprocess

BASE=Path(__file__).resolve().parent
BUILD=BASE/'.cache/build'
rows=[]

def measure(command,data,threads=1):
    """Проверить результат каждого запуска и вернуть пять внутренних измерений времени."""
    samples=[]
    for attempt in range(6):
        env=dict(os.environ,OMP_NUM_THREADS=str(threads),OMP_DYNAMIC='FALSE',OMP_PROC_BIND='FALSE')
        result=subprocess.run(command,input=data,text=True,capture_output=True,env=env,timeout=120)
        if result.returncode: raise RuntimeError(result.stderr)
        pi=re.search(r'pi = ([0-9.]+)',result.stdout)
        if pi and abs(float(pi[1])-math.pi)>=1e-8: raise AssertionError(result.stdout)
        value=float(re.search(r'Time = ([0-9.]+)',result.stdout)[1])
        if not value>0: raise AssertionError(result.stdout)
        if attempt: samples.append(value)
    return samples

def save():
    """Сохранить исходные измерения, из которых вычисляются таблицы отчёта."""
    path=BASE/'.cache/measurements.json'
    path.parent.mkdir(exist_ok=True)
    path.write_text(json.dumps({'measured_at':datetime.now().astimezone().isoformat(),
        'warmups':1,'measured_runs':5,'rows':rows},ensure_ascii=False,indent=2)+'\n')

word=('abcdefghijklmnopqrstuvwxyz'*4)[:100]
a=list(range(1,101))
b=[int(i==j) for i in range(10) for j in range(10)]
matrix='10\n'+' '.join(map(str,a+b))+'\n'
cases=[(17,'100\n'+word+'\n','строка, n=100'),
       (18,'1000000\n','pi, N=1000000'),
       (18,'100000000\n','pi, N=100000000'),
       (19,matrix,'матрица, n=10')]
for task,data,label in cases:
    for p in [1,2,4]:
        for point in [False,True]:
            command=['mpirun','--bind-to','none','-np',str(p),str(BUILD/f'task-{task}'),'--benchmark']
            if point: command.append('--point-to-point')
            samples=measure(command,data)
            row={'task':task,'case':label,'processes':p,
                 'mode':'point-to-point' if point else 'collective',
                 'seconds':samples,'median':statistics.median(samples)}
            rows.append(row)
            print(label,p,row['mode'],f"{row['median']:.9f}",flush=True)
            save()
