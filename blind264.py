### blind264 version 0.6 - intro to GUI
ver = '0.6'

import sys, os, subprocess, argparse, shlex, StringIO, time

def floatrange(i,j,k):
    x = range(int(100*i),int(100*j),int(100*k))
    y = list([z/100.0 for z in x])
    return y

class TestRig:
    def __init__(self, src, keep=False):
        self.source = src
        self.keep = keep
        self.results = {}
        self.params = {}
        self.avstext = []

        # Make logging directory
        shortname = os.path.basename(src).split('.')[0]
        try:
            os.mkdir(shortname)
        except:
            pass
        self.logdir = shortname + os.sep
        self.logfile = shortname + '.log'
        self.csvfile = shortname + 'blind264csv' + '.csv'

        # Write the first line of the log
        with open(self.logdir+self.logfile,'w') as l:
            l.write('blind264 version '+ver+'\n\n')
            
        with open(self.logdir+self.csvfile,'w') as csv:
            csv.write('blind264 version'+','+ver+','+'infile'+','+source+'\nqcomp,crf,SSIM,db,bitrate,fps,test time(in seconds)\n')
			
        # Read the .avs file into memory so we can use it to create our own test.avs
        with open(self.source,'r') as avs:
            while (True):
                line = avs.readline()
                if line == '':
                    break
                elif (line[:16].lower() == 'selectrangeevery'):
                    self.log('\nYOUR .AVS FILE CONTAINS A SELECTRANGEEVERY() LINE.  REMOVE IT AND TRY AGAIN.')
                    exit()
                else:
                    self.avstext.append(line)
                    
        # Find width, height, and frame count
        self.source = '"' + self.source + '"'
        args = shlex.split('pipebuf avs2yuv ' + self.source + ' -frames 1 -o - : x264 - --stdin y4m --output ')
        args.append(self.logdir+'test.mkv')
        p = subprocess.check_output(args,bufsize=-1,stderr=subprocess.STDOUT)
        line = StringIO.StringIO(p).readline()
        self.frames = int(line.split()[-2])
        res = line.split()[-5][:-1]
        res = res.split('x')
        self.width = int(res[0])
        self.height = int(res[1])

        self.set('ref',min(16,(8388608/(self.width*self.height))))
        self.set('ssim')

    def set(self, k, v=''):
        self.params[k] = str(v)
    def rem(self, k):
        if k in self.params:
            del self.params[k]
    def log(self,line):
        with open(self.logdir+self.logfile,'a') as l:
            l.write(line+'\n')
        print line

    def setrange(self,step=0,count=0,seek=0):
        with open(self.logdir+'test.avs', 'w') as out:
            for line in self.avstext:
                out.write(line)
            if not (step == 0 or count == 0):
                out.write('\nSelectRangeEvery('+str(step)+','+str(count)+','+str(seek)+')')
                self.log('\nSetting SelectRangeEvery('+str(step)+','+str(count)+','+str(seek)+')\n')
    def string(self):
        param_string = ''
        for k,v in self.params.iteritems():
            param_string += ' --' + k
            if v != '': param_string += ' ' + v
        return param_string
    def override(self,setting,tests,msg=''):
        if setting in tests:
            print 'Input a value to use for '+setting+', or press ENTER to use the computed value.'
            override = raw_input()
            print
            if override != '':
                self.set(setting,override)
                self.log('User chose '+setting+' '+override)
        else:
            override = ''
            while override == '':
                print 'Input a value to use for '+setting+'. '+msg
                override = raw_input()
            self.set(setting,override)
            self.log('User chose '+setting+' '+override)
    def test_encode(self,outfile='test.mkv',use_previous=True,verbose=True):
        self.rem('output')
        if self.string() in self.results and use_previous:
            info = self.results[self.string()]
            self.log('Using previously-recorded data from the following parameters:\n' + self.string())
            self.log('\nPreviously-recorded results:\nframes:\t\t' + str(info['frames']) +
                '\nssim:\t\t' + str(info['ssim']) + '\ndb:\t\t' + str(info['db']) + '\nbitrate:\t' +
                str(info['bitrate']) + '\nfps:\t\t' + str(info['fps']) + '\n')
            return info
        if self.keep or outfile.startswith('psy-rd'):
            self.set('output',self.logdir+outfile)
        else:
            self.set('output',self.logdir+'test.mkv')
            
        if verbose: self.log('Starting test encode with the following parameters:\n' + self.string())
        param_string = self.string()
        param_string = 'pipebuf avs2yuv '+self.logdir+'test.avs -o - : x264 - --stdin y4m' + param_string
        args = param_string.split()
        start = time.time()
        p = subprocess.check_output(args,bufsize=-1,stderr=subprocess.STDOUT)
        end = time.time()
        info = {}
        info['time'] = (end - start)
        workfile = StringIO.StringIO(p)
        while (True):
            line = workfile.readline()
            if line == '':
                break
            elif 'consecutive B-frames' in line:
                bframe_list = [float(s[:-1]) for s in line.split()[4:]]
                info['bframes'] = [0,bframe_list]
                for i in range(len(bframe_list)):
                    if bframe_list[i] >= 1.0: info['bframes'][0] = i
            elif 'SSIM Mean' in line:
                metric_list = line.split()
                info['ssim'] = float(metric_list[4][2:])
                info['db'] = float(metric_list[5][1:-3])
            elif line.startswith('encoded'):
                rate_list = line.split()
                info['frames'] = int(rate_list[1])
                info['fps'] = float(rate_list[3])
                info['bitrate'] = float(rate_list[5])
        if info == {}:
            self.log('\n'+workfile+'\nTEST ENCODE FAILED.')
            exit()
        self.log('\nFinished test encode with the following results:\nframes:\t\t' + str(info['frames']) + '\nssim:\t\t' + str(info['ssim']) +
                '\ndb:\t\t' + str(info['db']) + '\nbitrate:\t' + str(info['bitrate']) + '\nfps:\t\t' + str(info['fps']) + '\ntime:\t\t' + str(info['time']) + '\n')
        self.rem('output')
        self.results[self.string()] = info
        return info
    def bframes(self):
        self.log('\nRunning bframes test...\n')
        test = self.test_encode('baseline.mkv')
        b = test['bframes'][0]
        self.log('Consecutive bframes used: '+str(test['bframes'][1])+'\n\nblind264 chose bframes = ' + str(b) + '\n')
        self.set('bframes',b)
        self.baseline = test
    def trellis(self):
        self.log('\nRunning trellis test...\n')
        self.set('trellis',0)
        test = self.test_encode('trellis_0.mkv')
        if test['ssim'] > self.baseline['ssim']:
            self.log('Setting trellis to 0 improved SSIM.')
        else:
            self.set('trellis',2)
    def aq(self):
        self.log('\nRunning aq-mode test...\n')
        self.set('aq-mode',0)
        test = self.test_encode('aq-mode_0.mkv')
        if test['ssim'] > self.baseline['ssim']:
            self.log('Setting aq-mode to 0 improved SSIM.')
        else:
            self.set('aq-mode',2)
    def dct(self):
        self.log('\nRunning no-dct-decimate test...\n')
        self.set('no-dct-decimate')
        test = self.test_encode('no-dct-decimate.mkv')
        if test['ssim'] > self.baseline['ssim']:
            self.log('Setting no-dct-decimate improved SSIM.')
        else:
            self.rem('no-dct-decimate')
    def weightb(self):
        self.log('\nRunning no-weightb test...\n')
        self.set('no-weightb')
        test = self.test_encode('no-weightb.mkv')
        if test['ssim'] > self.baseline['ssim']:
            self.log('Setting no-weightb improved SSIM.')
        else:
            self.rem('no-weightb')
    def aqstrength(self):
        self.log('\nRunning aq-strength tests...\n')
        tests = []
        #test aq-strength in the range of start to end, in increments of step.
		##############################
        ##     AQS TEST VALUES      ##
        ##############################
        aqsstart = 1.3 if cli['grainy'] else 1.0
        aqsend = 0.8 if cli['grainy'] else 0.5
        aqsstep = -0.05
        ##############################
        for aqs in floatrange(aqsstart, aqsend+aqsstep, aqsstep):
            self.set('aq-strength',aqs)
            self.log('aq-strength = ' + str(aqs))
            tests.append(self.test_encode('aqstrength_'+str(aqs)+'.mkv'))
        # pick the aq-strength that corresponds to the largest deltaSSIM
        ssim_data = [x['ssim'] for x in tests]
        deltaSSIM = [10]*len(tests)
        for i in range(1,len(ssim_data)):
            deltaSSIM[i] = 100 * (ssim_data[i] - ssim_data[i-1]) / ssim_data[i-1]
        self.log('deltaSSIMs (as aq-strength decreases): '+str(deltaSSIM)+'\n')
        peak1 = deltaSSIM.index(max(deltaSSIM[1:]))
        peak1 = aqsstart + peak1*aqsstep
        self.log('blind264 chose aq-strength = ' + str(peak1) + '\n')
        self.set('aq-strength',peak1)
    def psyrdo(self,grainy):
        self.log('\nRunning psy-rdo tests...\n')
        tests = []
        # add --psy-rd start:0.0, and test in increments of step on the left, down to end
        ##############################
        ##    PSY-RDO TEST VALUES   ##
        ##############################
        start = 1.3
        end = 0.5 if grainy else 0.7
        step = -0.05
        ##############################
        for i in floatrange(start,end+step,step):
            self.set('psy-rd',str(i)+':0.0')
            self.log('psy-rd = '+str(i)+':0.0')
            tests.append(self.test_encode('psy-rd_'+str(i)+'_0.0.mkv'))
        print
        print 'There are several psy-rd test encode files in your blind264 directory.\n' + \
              'Which one looks the best? (enter the decimal number for psy-rdo ONLY, e.g., 0.9 if the file psy-rd_0.9_0.0.mkv looks the best)'
        psy = raw_input()
        self.log('\nUser chose psy-rdo '+psy+'\n')
        self.set('psy-rd',psy+':0.0')
    def psytrellis(self):
        self.log('\nRunning psy-trellis tests...\n')
        tests = []
        if self.params.has_key('psy-rd'):
            psyrdo = self.params['psy-rd'].split(':')[0]
        # add psy-trellis 0.0, and test in increments of step on the left, down to end
        ##############################
        ## PSY-TRELLIS TEST VALUES  ##
        ##############################
        start = 0.0
        end = 0.3
        step = 0.05
        ##############################
        for i in floatrange(start,end+step,step):
            self.set('psy-rd',psyrdo+':'+str(i))
            self.log('psy-rd = '+psyrdo+':'+str(i))
            tests.append(self.test_encode('psy-rd_'+psyrdo+'_'+str(i)+'.mkv'))
        print
        print 'There are several psy-rd test encode files in your blind264 directory.\n' + \
              'Which one looks the best? (enter the decimal number for psy-trellis ONLY, e.g., 0.05 if the file psy-rd_'+psyrdo+'_0.05.mkv looks the best)'
        psy = raw_input()
        self.log('\nUser chose psy-trellis '+psy+'\n')
        self.set('psy-rd',psyrdo+':'+psy)
    def crfqcomp(self):
		#Test crf and qcomp in a matrix and output a .csv of the data
        self.log('\nRunning qcomp/crf tests...\n')
        ##############################
		##############################
        ##    QCOMP TEST VALUES     ##
        ##############################
        qcstart = 0.8
        qcend = 0.6
        qcstep = -0.05
        ##############################
        ##############################
		##############################
        ##     CRF TEST VALUES      ##
        ##############################
        crfstart = 22
        crfend = 17
        crfstep = -0.5
        ##############################
        #The first setrange value was chosen as 'optimal' for DHSAB, the 2nd is for very fast testing to make sure everything works, the 3rd pulls the setrange info from the config file.
        #Remove the '#' from one (and only one) of these three setrange lines.		
        #setrange(avstext,4762,310,4263)
        #self.setrange(self.avstext,20000,10,10000)
        #setrange(avstext,trange[0],trange[1],trange[2])
        for qcomp in floatrange(qcstart, qcend+qcstep, qcstep):
            self.set('qcomp',qcomp)
            for crf in floatrange(crfstart, crfend+crfstep, crfstep):
                self.set('crf',crf)
                self.log('qcomp = ' + str(qcomp) + ', crf = ' + str(crf))
                test = self.test_encode('qcomp_'+str(qcomp)+'_crf_'+str(crf)+'.mkv',use_previous=False,verbose=False)
                with open(self.logdir+self.csvfile,'a') as csv:
                    csv.write(str(qcomp)+','+str(crf)+','+str(test['ssim'])+','+str(test['db'])+','+str(test['bitrate'])+','+str(test['fps'])+','+str(test['time'])+'\n')
                csv.closed

if __name__ == '__main__':
    ################# CLI OPTIONS ##################
    TESTS = ('bframes','trellis','aq-mode','no-dct-decimate','no-weightb','crf','psy-rd','merange','crfqcomp','aqstrength')
    supported_tests = set(['bframes','trellis','aq-mode','no-dct-decimate','no-weightb','crf','psy-rd','crfqcomp','aqstrength'])
    supported_override = set(['bframes','crf','psy-rd','me'])

    parser = argparse.ArgumentParser(description='blind264 - Automated test encoding', usage='%(prog)s infile.avs [options]')
    parser.add_argument('--version', action='version', version='%(prog)s version '+ver)
    parser.add_argument('infile', help='source avisynth script')
    parser.add_argument('--hd', action='store_true', help='set this flag if the source is high definition')
    parser.add_argument('--grainy', action='store_true', help='set this flag if the source is grainy')
    parser.add_argument('--keep', action='store_true', help = 'set this flag if you want to keep all test encodes for manual \
                        comparison (this may take up lots of space)')
    parser.add_argument('--sar', default=argparse.SUPPRESS, help='sample aspect ratio - mandatory for SD sources')
    parser.add_argument('--override', metavar='test', nargs='+', default=[], help='this option allows you to override the \
                        settings picked by the specified tests')
    parser.add_argument('--only', metavar='test', dest='test', nargs='+', help='this option overrides the standard procedure \
                        and only runs the specified tests')

    cli = vars(parser.parse_args())


    ################## CLI CHECKS ##################
    if not os.path.isfile(cli['infile']):
        parser.exit(status=2, message='\nError: Input file ' + cli['infile'] + ' does not exist.')
    elif not cli['infile'].endswith('.avs'):
        parser.exit(status=2, message='\nError: Input file must be an avisynth script (*.avs).')
    else:
        source = cli['infile']

    if (not cli['hd']) and ('sar' not in cli):
        parser.exit(status=2, message='\nError: SAR required for SD sources.')

    if cli['test'] is None:
        cli['test'] = set(['bframes','psy-rd','crfqcomp','aqstrength'])
        if cli['grainy']:
            cli['test'].add('trellis')
            cli['test'].add('no-dct-decimate')
            cli['test'].add('no-weightb')
    else:
        for x in cli['test']:
            if x not in supported_tests: parser.exit(status=2, message='\nError: unsupported test '+x+'.')
        cli['test'] = set(cli['test'])
        for x in cli['override']:
            if x not in supported_override: parser.exit(status=2, message='\nError: unsupported override '+x+'.')
        cli['override'] = set(cli['override'])


    ################# INITIAL X264 PARAMETERS #################
    x264 = TestRig(source, keep=cli['keep'])
    if 'sar' in cli:
        x264.set('sar',cli['sar'])

    from config import *
    config = startingOptions(x264)
    brange = config[0]
    trange = config[1]


    ############### TEST ENCODING PROCEDURE #################
    prologue = 'STARTING TEST ENCODING PROCESS FOR:\n' + source + '\n\nCommand line options:\n'+str(sys.argv[1:])+'\n\nParsed options:\n'
    for k,v in cli.iteritems():
        prologue += k + ': ' + str(v) + '\n'
    prologue += '\nSource width: '+str(x264.width)+'\nSource height: '+str(x264.height)+'\nFrame count: '+str(x264.frames)+'\n'
    x264.log(prologue)

    if ('bframes' in cli['test']):
        x264.setrange(20000,10,10000)
        #x264.setrange(brange[0],brange[1],brange[2])
        x264.bframes()
    if ('bframes' in cli['override']):
        x264.override('bframes',cli['test'])

    #x264.setrange(trange[0],trange[1],trange[2])
    x264.setrange(20000,10,10000)
    print

    if ('trellis' in cli['test']):
        x264.trellis()

    if ('aq-mode' in cli['test']):
        x264.aq()

    if ('no-dct-decimate' in cli['test']):
        x264.dct()

    if ('no-weightb' in cli['test']):
        x264.weightb()

    if ('crf' in cli['test']):
        x264.crf()
    if ('crf' in cli['override']):
        x264.override('crf',cli['test'])

    if ('psy-rd' in cli['test']):
        x264.psyrdo(cli['grainy'])
    elif ('psy-rd' in cli['override']):
        x264.override('psy-rd',cli['test'],'\nINCLUDE NUMBERS FOR BOTH RDO AND TRELLIS (e.g., 1.0:0.5)')

    if ('psy-rd' in cli['test'] and not cli['grainy']):
        x264.psytrellis()

    if ('crfqcomp' in cli['test']):
        x264.crfqcomp()

    if ('aqstrength' in cli['test']):
        x264.aqstrength()

    ### DONE ###
    x264.rem('ssim')
    x264.rem('output')
    if ('me' in cli['override']):
        x264.set('merange',24)
    else:
        x264.set('me','tesa')
        x264.set('merange',16)
    x264.log('\n\nTESTING COMPLETE! USE THE FOLLOWING PARAMETERS FOR YOUR ENCODE:\n'+x264.string())
