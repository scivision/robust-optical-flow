project('gnc', 'c',
  default_options : ['default_library=static', 'buildtype=release', 'warning_level=0'])

cc = meson.get_compiler('c')

libm = cc.find_library('m')

subdir('src')

gnc_exe = executable('gnc', gnc_src,
  dependencies: libm)

test('basic', gnc_exe,
  args: ['11', '12', '4', '1', 'data/yos', 'results/flow-', '-l1', '10.0', '-l2', '1.0', '-S1', '10.0', '-S2', '1.0', '-s1', '10.0', '-s2', '0.05', '-stages', '5', '-nx', '316', '-ny', '252', '-f', '0.5', '-F', '0', '-w', '1.995', '-iters', '20', '-end', '.pgm', '-skip', '15'],
  workdir: meson.source_root(),
  timeout: 15)

octave = find_program('octave', required: false)

if octave.found()
  test('Matlab', octave,
    args: ['--eval', '"BlackRobustFlow(\'data/pepsi\')"'],
    workdir: meson.source_root(),
    timeout: 60)
endif
