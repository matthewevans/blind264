
### DON'T EDIT THESE UNLESS YOU KNOW WHAT YOU'RE DOING.

def startingOptions(x264):
    x264.set('crf',17.0)
    x264.set('direct','auto')
    x264.set('bframes',16)
    x264.set('b-adapt',2)
    x264.set('b-pyramid','normal')
    x264.set('psy-rd','1.0:0.0')
    x264.set('deblock','-3:-3')
    x264.set('subme',10)
    x264.set('trellis',2)
    x264.set('me','umh')
    x264.set('merange',24)
    x264.set('aq-mode',1)
    x264.set('trellis',2)
    x264.set('qcomp',0.7)
    x264.set('rc-lookahead',250)
    x264.set('analyse','all')
    x264.set('no-fast-pskip')
    x264.set('thread-input')
    
    bframesSelectRangeEvery = (20000,1300,10000)
    otherSelectRangeEvery = (20000,500,10000)
    
    return (bframesSelectRangeEvery, otherSelectRangeEvery)