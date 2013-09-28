import sys
from stream_summary import StreamSummary

def main():
    if len(sys.argv) != 3:
        print ("Usage ss_test.py <StreamSummary Size> <file_name>")
        return

    size = sys.argv[1]

    with open(sys.argv[2], "r") as f:
        data = f.read().splitlines()

    data = [x for x in data if not x.startswith('#') and not len(x) == 0]
    ss = StreamSummary(int(size))
    
    for e in data:
        ss.add(e)

    print(ss)
    

if __name__ == "__main__":
    main()
