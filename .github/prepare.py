import datetime
import json
import os
import re
import sys
import subprocess 
def set_output(key, value):
    with open(os.getenv('GITHUB_OUTPUT'), 'a') as f:
        f.write(f"{key}={value}\n")
# 手动输入的版本号
version = os.getenv('INPUT_VERSION').strip()
if not version:
    raise SystemExit("No version provided")
# 去除前缀 'v' 并提取主版本和是否为 beta
version = version.lstrip('v')
is_beta = version.endswith('b')
version_base = version.split('-')[0]
parts = version_base.split('.')
if len(parts) != 3:
    raise SystemExit(f"Invalid version format: {version}. Expected x.x.x or x.x.xb")
major, minor, patch = parts
display_version = f"{major}.{minor}.{patch}"
if is_beta:
    display_version += "b"
current_date = datetime.datetime.now().strftime('%Y-%m-%d')
release_type = 'beta' if is_beta else 'stable'
release_name = f'{display_version} ({current_date})'
# 输出基本版本和发行信息
set_output('version', display_version)
set_output('release_type', release_type)
set_output('release_name', release_name)
set_output('do_release', 'yes')
set_output('do_publish', 'yes')
# 加载 meson build options 和 project info
subprocess.run(['meson', 'setup', '-Dprepare=true', 'build-prepare'], check=True)
with open('build-prepare/meson-info/intro-buildoptions.json') as f:
    build_options = {opt['name']: opt['value'] for opt in json.load(f)}
with open('build-prepare/meson-info/intro-projectinfo.json') as f:
    project_info = json.load(f)
    app_name = project_info['name']
    app_vendor = build_options.get('app_vendor', 'unknown')
    app_data = build_options.get('app_data', 'unknown')
mod_id = build_options.get('mod_id', '0')
try:
    mod_id = int(mod_id)
except ValueError:
    mod_id = 0
if mod_id == 0:
    app_name_full = app_name
    app_comment = build_options['app_comment']
    app_exe = build_options['app_exe']
    app_id = build_options['app_id']
    if is_beta:
        app_name_full += " Beta"
        app_comment += " - Beta"
        app_exe += 'beta'
        app_id += 'beta'
    else:
        app_name_full += " Stable"
    build_options['app_name'] = app_name_full
    build_options['app_comment'] = app_comment
    build_options['app_exe'] = app_exe
    build_options['app_id'] = app_id
# 写出其他输出变量
set_output('mod_id', str(mod_id))
set_output('app_name', build_options['app_name'])
set_output('app_comment', build_options['app_comment'])
set_output('app_exe', build_options['app_exe'])
set_output('app_id', build_options['app_id'])
set_output('app_data', build_options['app_data'])
set_output('app_vendor', build_options['app_vendor'])

app_exe = build_options['app_exe']
app_name = build_options['app_name']
app_name_slug = re.sub('[^A-Za-z0-9]', '_', app_name)

build_matrix = []
publish_matrix = []
# consider disabling line wrapping to edit this monstrosity
for        arch,     platform,         libc,   statdyn, bplatform,         runson, suffix,          reason, dbgsuffix,         mode,              starcatcher,    dbgrel, priority,  lint in [
	(  'x86_64',      'linux',        'gnu',  'static',   'linux', 'ubuntu-22.04',     '',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: static debug build
	(  'x86_64',      'linux',        'gnu',  'static',   'linux', 'ubuntu-22.04',     '',       'publish',    '.dbg',         None,  'x86_64-lin-gcc-static', 'release',       10, False ),
	(  'x86_64',      'linux',        'gnu',  'static',   'linux', 'ubuntu-22.04',     '',       'publish',    '.dbg',      'steam',  'x86_64-lin-gcc-static', 'release',       -5, False ), # priority = -5: steam build
	(  'x86_64',      'linux',        'gnu',  'static',   'linux', 'ubuntu-22.04',     '',       'archive',    '.dbg',   'appimage',                     None, 'release',        0, False ), # priority = 0: appimage release
	(  'x86_64',      'linux',        'gnu', 'dynamic',   'linux', 'ubuntu-22.04',     '',         'check',      None,         None,                     None,   'debug',       10,  True ), # the only debug job that lints; this covers the font editor and the renderer
	(  'x86_64',      'linux',        'gnu', 'dynamic',   'linux', 'ubuntu-22.04',     '',         'check',      None,     'nohttp',                     None,   'debug',       10, False ),
	(  'x86_64',      'linux',        'gnu', 'dynamic',   'linux', 'ubuntu-22.04',     '',         'check',      None,      'nolua',                     None,   'debug',       10, False ),
	(  'x86_64',      'linux',        'gnu', 'dynamic',   'linux', 'ubuntu-22.04',     '',         'check',      None,         None,                     None, 'release',       10, False ),
	( 'aarch64',      'linux',        'gnu',  'static',   'linux', 'ubuntu-22.04-arm', '',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: static debug build
	( 'aarch64',      'linux',        'gnu',  'static',   'linux', 'ubuntu-22.04-arm', '',       'publish',    '.dbg',         None,   'arm64-lin-gcc-static', 'release',       10, False ),
	( 'aarch64',      'linux',        'gnu',  'static',   'linux', 'ubuntu-22.04-arm', '',       'archive',    '.dbg',   'appimage',                     None, 'release',        0, False ), # priority = 0: appimage release
	( 'aarch64',      'linux',        'gnu', 'dynamic',   'linux', 'ubuntu-22.04-arm', '',         'check',      None,         None,                     None,   'debug',       10, False ),
	( 'aarch64',      'linux',        'gnu', 'dynamic',   'linux', 'ubuntu-22.04-arm', '',         'check',      None,         None,                     None, 'release',       10, False ),
#	(  'x86_64',    'windows',      'mingw',  'static',   'linux', 'ubuntu-22.04',     '',         'check',      None,         None,                     None,   'debug',       10, False ), # ubuntu-22.04 doesn't have windows TLS headers somehow and I haven't yet figured out how to get them; worse, it's a different toolchain
#	(  'x86_64',    'windows',      'mingw',  'static',   'linux', 'ubuntu-22.04',     '',       'archive',    '.dbg',         None,                     None, 'release',       10, False ), # ubuntu-22.04 doesn't have windows TLS headers somehow and I haven't yet figured out how to get them; worse, it's a different toolchain
#	(  'x86_64',    'windows',      'mingw', 'dynamic',   'linux', 'ubuntu-22.04',     '',         'check',      None,         None,                     None,   'debug',       10, False ), # ubuntu-22.04 doesn't have ucrt64-capable mingw >_>
#	(  'x86_64',    'windows',      'mingw', 'dynamic',   'linux', 'ubuntu-22.04',     '',         'check',      None,         None,                     None, 'release',       10, False ), # ubuntu-22.04 doesn't have ucrt64-capable mingw >_>
	(  'x86_64',    'windows',      'mingw',  'static', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: static debug build
	(  'x86_64',    'windows',      'mingw',  'static', 'windows', 'windows-2022', '.exe',       'archive',    '.dbg',         None,                     None, 'release',       10, False ),
	(  'x86_64',    'windows',      'mingw', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None,   'debug',       10, False ),
	(  'x86_64',    'windows',      'mingw', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None, 'release',       10,  True ),
	(  'x86_64',    'windows',       'msvc',  'static', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: static debug build
	(  'x86_64',    'windows',       'msvc',  'static', 'windows', 'windows-2022', '.exe',       'publish',    '.pdb',         None, 'x86_64-win-msvc-static', 'release',       10, False ),
	(  'x86_64',    'windows',       'msvc',  'static', 'windows', 'windows-2022', '.exe',       'publish',    '.pdb',      'steam', 'x86_64-win-msvc-static', 'release',       -5, False ), # priority = -5: steam build
	(  'x86_64',    'windows',       'msvc', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None,   'debug',       10, False ),
#	(  'x86_64',    'windows',       'msvc', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,  'backendvs',                     None,   'debug',        0, False ), # priority = 0: backend=vs build
	(  'x86_64',    'windows',       'msvc', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None, 'release',       10, False ),
	(     'x86',    'windows',       'msvc',  'static', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: static debug build
	(     'x86',    'windows',       'msvc',  'static', 'windows', 'windows-2022', '.exe',       'publish',    '.pdb',         None,   'i686-win-msvc-static', 'release',       10, False ),
	(     'x86',    'windows',       'msvc', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None,   'debug',       10, False ),
	(     'x86',    'windows',       'msvc', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None, 'release',       10, False ),
	( 'aarch64',    'windows',       'msvc',  'static', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: static debug build
	( 'aarch64',    'windows',       'msvc',  'static', 'windows', 'windows-2022', '.exe',       'publish',    '.pdb',         None,  'arm64-win-msvc-static', 'release',       10, False ),
	( 'aarch64',    'windows',       'msvc', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None,   'debug',       10, False ),
	( 'aarch64',    'windows',       'msvc', 'dynamic', 'windows', 'windows-2022', '.exe',         'check',      None,         None,                     None, 'release',       10, False ),
	(  'x86_64',     'darwin',      'macos',  'static',  'darwin',     'macos-13', '.dmg',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: static debug build
	(  'x86_64',     'darwin',      'macos',  'static',  'darwin',     'macos-13', '.dmg',       'publish',      None,         None,  'x86_64-mac-gcc-static', 'release',       10, False ), # I have no idea how to separate debug info on macos
	(  'x86_64',     'darwin',      'macos',  'static',  'darwin',     'macos-13', '.dmg',       'publish',      None,      'steam',  'x86_64-mac-gcc-static', 'release',       -5, False ), # priority = -5: steam build, see above regarding debug info
	(  'x86_64',     'darwin',      'macos', 'dynamic',  'darwin',     'macos-13', '.dmg',         'check',      None,         None,                     None,   'debug',       10, False ),
	(  'x86_64',     'darwin',      'macos', 'dynamic',  'darwin',     'macos-13', '.dmg',         'check',      None,         None,                     None, 'release',       10, False ), # TODO: enable lint once apple clang ships clang-tidy
	( 'aarch64',     'darwin',      'macos',  'static',  'darwin',     'macos-13', '.dmg',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: static debug build
	( 'aarch64',     'darwin',      'macos',  'static',  'darwin',     'macos-13', '.dmg',       'publish',      None,         None,   'arm64-mac-gcc-static', 'release',       10, False ),
#	( 'aarch64',     'darwin',      'macos', 'dynamic',  'darwin',     'macos-13', '.dmg',         'check',      None,         None,                     None,   'debug',       10, False ), # macos-11.0 is x86_64 and I haven't yet figured out how to get homebrew to install aarch64 libs on x86_64
#	( 'aarch64',     'darwin',      'macos', 'dynamic',  'darwin',     'macos-13', '.dmg',         'check',      None,         None,                     None, 'release',       10, False ), # macos-11.0 is x86_64 and I haven't yet figured out how to get homebrew to install aarch64 libs on x86_64
	(     'x86',    'android',     'bionic',  'static',   'linux', 'ubuntu-22.04', '.apk',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: rarely used debug build
	(     'x86',    'android',     'bionic',  'static',   'linux', 'ubuntu-22.04', '.apk',       'publish',    '.dbg',         None,    'i686-and-gcc-static', 'release',       10, False ),
	(  'x86_64',    'android',     'bionic',  'static',   'linux', 'ubuntu-22.04', '.apk',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: rarely used debug build
	(  'x86_64',    'android',     'bionic',  'static',   'linux', 'ubuntu-22.04', '.apk',       'publish',    '.dbg',         None,  'x86_64-and-gcc-static', 'release',       10, False ),
	(     'arm',    'android',     'bionic',  'static',   'linux', 'ubuntu-22.04', '.apk',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: rarely used debug build
	(     'arm',    'android',     'bionic',  'static',   'linux', 'ubuntu-22.04', '.apk',       'publish',    '.dbg',         None,     'arm-and-gcc-static', 'release',       10, False ),
	( 'aarch64',    'android',     'bionic',  'static',   'linux', 'ubuntu-22.04', '.apk',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: rarely used debug build
	( 'aarch64',    'android',     'bionic',  'static',   'linux', 'ubuntu-22.04', '.apk',       'publish',    '.dbg',         None,   'arm64-and-gcc-static', 'release',       10,  True ),
	(  'wasm32', 'emscripten', 'emscripten',  'static',   'linux', 'ubuntu-22.04', '.tar',         'check',      None,         None,                     None,   'debug',        0, False ), # priority = 0: rarely used debug build
	(  'wasm32', 'emscripten', 'emscripten',  'static',   'linux', 'ubuntu-22.04', '.tar',       'publish','.dbg.wasm','emscripten',      'wasm32-ems-static', 'release',       10, False ), # TODO: enable lint once emscripten ships clang-tidy
]:
	if priority < do_priority:
		continue
	job_name = f'build'
	if starcatcher:
		job_name += f'+target=starcatcher-{starcatcher}'
	else:
		job_name += f'+target={arch}-{platform}-{libc}-{statdyn}-{dbgrel}'
		if mode:
			job_name += f'+mode={mode}'
		if bplatform != platform:
			job_name += f'+bplatform={bplatform}'
	if not mode:
		mode = 'default'
	separate_debug = True
	if not dbgsuffix:
		dbgsuffix = 'BOGUS'
		separate_debug = False
	if not starcatcher:
		starcatcher = 'BOGUS'
	job_release_name = release_name
	if mode == 'steam':
		if not steam_builds:
			continue
		job_release_name += '+steam'
		job_name += '+steam'
	artifact = False
	publish = False
	if reason == 'publish':
		artifact = True
		publish = True
	if reason == 'archive':
		artifact = True
	if dbgrel != 'release':
		assert not publish
		assert not artifact
	asset_path = f'{app_exe}{suffix}'
	asset_name = f'{app_exe}-{job_release_name}-{arch}-{platform}-{libc}{suffix}'
	debug_asset_path = f'{app_exe}{dbgsuffix}'
	debug_asset_name = f'{app_exe}-{job_release_name}-{arch}-{platform}-{libc}{dbgsuffix}'
	if mode	== 'appimage':
		asset_path = f'{app_name_slug}-{arch}.AppImage'
		asset_name = f'{app_name_slug}-{arch}.AppImage'
		debug_asset_path = f'{app_name_slug}-{arch}.AppImage.dbg'
		debug_asset_name = f'{app_name_slug}-{arch}.AppImage.dbg'
	starcatcher_name = f'powder-{job_release_name}-{starcatcher}{suffix}'
	msys2_bash = (bplatform == 'windows' and libc == 'mingw')
	shell = 'bash'
	if msys2_bash:
		shell = 'msys2 {0}'
	item = {
		'bsh_build_platform': bplatform, # part of the unique portion of the matrix
		'bsh_host_arch': arch, # part of the unique portion of the matrix
		'bsh_host_platform': platform, # part of the unique portion of the matrix
		'bsh_host_libc': libc, # part of the unique portion of the matrix
		'bsh_static_dynamic': statdyn, # part of the unique portion of the matrix
		'bsh_debug_release': dbgrel, # part of the unique portion of the matrix
		'bsh_lint': 'no',
		'runs_on': runson,
		'force_msys2_bash': msys2_bash and 'yes' or 'no',
		'package_suffix': suffix,
		'package_mode': mode,
		'publish': publish and 'yes' or 'no',
		'artifact': artifact and 'yes' or 'no',
		'separate_debug': separate_debug and 'yes' or 'no',
		'asset_path': asset_path,
		'asset_name': asset_name,
		'debug_asset_path': debug_asset_path,
		'debug_asset_name': debug_asset_name,
		'job_name': job_name,
		'shell': shell,
	}
	build_matrix.append(item)
	if lint:
		lint_item = item.copy()
		lint_item['publish'] = 'no'
		lint_item['artifact'] = 'no'
		lint_item['separate_debug'] = 'no'
		lint_item['bsh_lint'] = 'yes'
		if lint_item['runs_on'] == 'ubuntu-22.04':
			lint_item['runs_on'] = 'ubuntu-24.04'
		lint_item['job_name'] = lint_item['job_name'].replace('build', 'lint', 1)
		build_matrix.append(lint_item)
	if publish:
		publish_matrix.append({
			'bsh_build_platform': bplatform, # part of the unique portion of the matrix
			'bsh_host_arch': arch, # part of the unique portion of the matrix
			'bsh_host_platform': platform, # part of the unique portion of the matrix
			'bsh_host_libc': libc, # part of the unique portion of the matrix
			'bsh_static_dynamic': statdyn, # part of the unique portion of the matrix
			'asset_path': asset_path,
			'asset_name': asset_name,
			'starcatcher_name': starcatcher_name,
		})

set_output('build_matrix', json.dumps({ 'include': build_matrix }))
set_output('publish_matrix', json.dumps({ 'include': publish_matrix }))
set_output('do_release', 'yes')
set_output('do_publish', 'yes')
