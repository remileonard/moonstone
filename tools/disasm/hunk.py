import struct
TYPES={0x3E9:'CODE',0x3EA:'DATA',0x3EB:'BSS'}
def parse(path):
    d=open(path,'rb').read(); p=0
    def L():
        nonlocal p; v=struct.unpack('>I',d[p:p+4])[0]; p+=4; return v
    assert L()==0x3F3
    while True:
        n=L()
        if n==0: break
        p+=4*n
    n=L(); first=L(); last=L()
    hdr=[L() for _ in range(last-first+1)]
    hunks=[]; cur=None
    while p<len(d):
        t=L(); tt=t&0x3FFFFFFF
        if tt in TYPES:
            sz=L()*4
            mem={0:'',1:'CHIP',2:'FAST'}[hdr[len(hunks)]>>30]
            cur={'type':TYPES[tt],'mem':mem,'size':(hdr[len(hunks)]&0x3FFFFFFF)*4,
                 'data':d[p:p+sz] if tt!=0x3EB else b'','relocs':{}}
            if tt!=0x3EB: p+=sz
            hunks.append(cur)
        elif tt==0x3EC:
            while True:
                c=L()
                if c==0: break
                tgt=L()
                for _ in range(c): cur['relocs'][L()]=tgt
        elif tt==0x3F7: # reloc32short
            while True:
                c=struct.unpack('>H',d[p:p+2])[0]; p+=2
                if c==0: break
                tgt=struct.unpack('>H',d[p:p+2])[0]; p+=2
                for _ in range(c):
                    cur['relocs'][struct.unpack('>H',d[p:p+2])[0]]=tgt; p+=2
            if p%4: p+=2
        elif tt==0x3F0: # symbols
            while True:
                n=L()
                if n==0: break
                p+=4*n+4
        elif tt==0x3F1: p+=4*L()
        elif tt==0x3F2: pass
        else: raise Exception(f"hunk {t:x} @ {p}")
    return hunks
def compare(a,b):
    A,B=parse(a),parse(b); bad=0
    if len(A)!=len(B): print("  nb hunks",len(A),len(B)); return False
    for i,(x,y) in enumerate(zip(A,B)):
        pb=[]
        if (x['type'],x['mem'])!=(y['type'],y['mem']): pb.append(f"type {x['type']}{x['mem']}/{y['type']}{y['mem']}")
        if x['size']!=y['size']: pb.append(f"taille {x['size']}/{y['size']}")
        dx,dy=x['data'].ljust(x['size'] if x['type']!='BSS' else 0,b'\0'),y['data'].ljust(y['size'] if y['type']!='BSS' else 0,b'\0')
        if dx!=dy:
            k=next((j for j in range(min(len(dx),len(dy))) if dx[j]!=dy[j]),min(len(dx),len(dy)))
            pb.append(f"contenu diffère à +${k:X}")
        if x['relocs']!=y['relocs']: pb.append(f"relocs {len(x['relocs'])}/{len(y['relocs'])}")
        if pb: bad+=1; print(f"  hunk {i}: "+", ".join(pb))
    return bad==0
def diff_offsets(a,b):
    """Premier octet différent de chaque hunk : [(hunk, offset)]."""
    A,B=parse(a),parse(b); res=[]
    for i,(x,y) in enumerate(zip(A,B)):
        dx,dy=x['data'],y['data']
        n=min(len(dx),len(dy))
        k=next((j for j in range(n) if dx[j]!=dy[j]),None)
        if k is None and (len(dx)!=len(dy) or x['relocs']!=y['relocs']):
            k=min([n]+[o for o in set(x['relocs'])^set(y['relocs'])])
        if k is not None: res.append((i,k))
    return res
if __name__=='__main__':
    import sys; print("IDENTIQUE" if compare(sys.argv[1],sys.argv[2]) else "DIFFERENT")
