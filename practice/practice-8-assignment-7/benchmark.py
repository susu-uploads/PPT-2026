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

MPI_BUILD=BASE.parent/'practice-7-assignment-6/.cache/build'
configurations=[('serial',1,1)]+[('openmp',1,k) for k in [1,2,4,8,14]]
configurations += [('mpi',p,1) for p in [1,2,4,7,14]]
configurations += [('hybrid',p,k) for p,k in
    [(1,1),(1,2),(1,4),(1,8),(1,14),(2,1),(2,2),(2,4),(2,7),(4,1),(4,2),(7,2),(14,1)]]
for n in [1000000,100000000]:
    for mode,p,k in configurations:
        if mode=='mpi':
            command=[str(MPI_BUILD/'task-18'),'--benchmark']
        else:
            command=[str(BUILD/'task-32'),'--mode',mode]
        command=['mpirun','--bind-to','none','-np',str(p),*command]
        samples=measure(command,f'{n}\n',threads=k)
        row={'n':n,'mode':mode,'processes':p,'threads':k,
             'seconds':samples,'median':statistics.median(samples)}
        rows.append(row)
        print(n,mode,p,k,f"{row['median']:.9f}",flush=True)
        save()
