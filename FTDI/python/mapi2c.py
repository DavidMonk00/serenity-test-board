import subprocess

PATH = '/home/dmonk/serenity-test-board/FTDI'


def configMux():
    data = b'0x80'
    subprocess.run([
        PATH + '/bin/main',
        '-a', '0xE0',
        '-d', data,
        '-w'
    ])
    stdout = subprocess.check_output([
        PATH + '/bin/main',
        '-a', '0xE0',
        '-r'
    ])
    print(stdout)
    return data in stdout


def testAddress(address):
    return subprocess.run([
        PATH + '/bin/main',
        '-a', address,
        '-r',
        '>', 'out.txt'
    ])


def main():
    if configMux():
        for i in range(256):
            print(hex(i))
            try:
                print(testAddress(hex(i)).decode('ascii'))
            except Exception as e:
                pass


if __name__ == '__main__':
    main()
