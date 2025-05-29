from os.path import expanduser
from SCons.Script import *
import glob

homedir = expanduser('~')
path = ['/usr/local/bin', '/bin', '/usr/bin', f'{homedir}/.local/bin', '/ios-gaming/bin']

env = Environment(
    RANLIB='',
    AR='arm-apple-darwin9-ar',
    CC='arm-apple-darwin9-clang',
    CXX='arm-apple-darwin9-clang++',
    CFLAGS=[
        "-isysroot", "/home/electimon/Projects/ios-sdks/sdk3",
        "-arch", "armv6",
        "-fblocks",
        "-fPIC",
        "-fobjc-arc",
        "-I/home/electimon/sdk3/usr/include",
        "-IExternal/openssl/Headers",
        "-IHeaders",
        "-miphoneos-version-min=3.1.3",
        "-include", "Source/Prefix.pch",
    ],
    LINKFLAGS=[
        "-arch", "armv6",
        "-isysroot", "/home/electimon/Projects/ios-sdks/sdk3",
        "-framework", "Foundation",
        "-dynamiclib",
        "-miphoneos-version-min=3.1.3",
        "-LExternal/libclosure",
        "-LExternal/libdispatch",
        "-LExternal/openssl",
        "-ldispatch",
        "-lsystem_blocks",
        "-lssl",
        "-lcrypto",
    ],
    ENV={'PATH': path},
    SHLIBPREFIX='',
    SHLIBSUFFIX='',
)

# Main Prog
srcs = glob.glob('**/**.m', recursive=True) + glob.glob('**/**.c', recursive=True) + glob.glob('**/**.cpp', recursive=True)
def disable_arc(src_file_name):
    try:
        idx = next(i for i, s in enumerate(srcs) if src_file_name in s)
    except StopIteration:
        print(f"File {src_file_name} not found in srcs")
        return
    srcs[idx] = env.SharedObject(
        srcs[idx],
        CFLAGS=env['CFLAGS'] + ['-fno-objc-arc']
    )

disable_arc('ARC.m')
disable_arc('NSBlock.m')
disable_arc('NSJSONSerialization.m')
disable_arc('NSData-Base64.m')
disable_arc('Sort.m')

lib = env.SharedLibrary(target='YZFoundation.framework/YZFoundation', source=srcs)
env.Default(lib)

header_srcs = glob.glob('Source/**.h', recursive=True)
# copy Headers/ wholesale
header_srcs += glob.glob('Headers/*')
env.Command('YZFoundation.framework/Headers', header_srcs, 'mkdir -p $TARGET;cp -r $SOURCES $TARGET')
env.Depends('YZFoundation.framework/YZFoundation', "YZFoundation.framework/Headers")
env.Clean('YZFoundation.framework/YZFoundation', 'YZFoundation.framework')
