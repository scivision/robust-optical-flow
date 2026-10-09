classdef TestRobustFunctionClassdef < matlab.unittest.TestCase

  methods (TestClassSetup)
    function addFlowCodePath(testCase)
      import matlab.unittest.fixtures.PathFixture
      flowCodeDir = fileparts(fileparts(mfilename('fullpath')));
      testCase.applyFixture(PathFixture(flowCodeDir))
      testCase.applyFixture(PathFixture( ...
        fullfile(flowCodeDir, 'utils'), IncludingSubfolders=true))
    end
  end

  methods (Test)
    function testDefaultCopyAndSigmaAlias(testCase)
      defaultModel = robust_function;
      testCase.verifyEqual(func2str(defaultModel.type), 'quadratic')
      testCase.verifyEqual(defaultModel.param, 1)
      testCase.verifyEqual(defaultModel.sigma, 1)

      model = robust_function('huber', 0.7);
      copiedModel = robust_function(model);
      copiedModel.sigma = 2;
      testCase.verifyEqual(model.param, 0.7)
      testCase.verifyEqual(model.sigma, 0.7)
      testCase.verifyEqual(copiedModel.param, 2)
    end

    function testSupportedPenaltyConstructors(testCase)
      names = {'geman_mcclure', 'huber', 'lorentzian', 'quadratic', ...
        'tukey', 'spline', 'mixture', 'gaussian', 'tdist', ...
        'tdist_unnorm', 'charbonnier', 'generalized_charbonnier'};

      for i = 1:numel(names)
        model = robust_function(names{i});
        testCase.verifyEqual(func2str(model.type), names{i})
      end

      generalized = robust_function('generalized_charbonnier');
      testCase.verifyEqual(generalized.param, [1e-3, 1])
      customGeneralized = robust_function('generalized_charbonnier', 0.01, 0.45);
      testCase.verifyEqual(customGeneralized.param, [0.01, 0.45])
      stringName = robust_function("huber", 0.7);
      testCase.verifyEqual(func2str(stringName.type), 'huber')
      testCase.verifyEqual(stringName.param, 0.7)
    end

    function testInvalidConstructorInputs(testCase)
      testCase.verifyError(@() robust_function('unknown'), ...
        'robust_function:InvalidArguments')
      testCase.verifyError(@() robust_function(42), ...
        'robust_function:InvalidArguments')
      testCase.verifyError(@() robust_function('huber', 1, 2), ...
        'robust_function:InvalidArguments')
      testCase.verifyError(@() robust_function('generalized_charbonnier', 1), ...
        'robust_function:InvalidArguments')
      testCase.verifyError(@() robust_function('huber', 1, 2, 3), ...
        'MATLAB:TooManyInputs')
    end

    function testPenaltyEvaluation(testCase)
      model = robust_function('huber', 0.7);
      x = [-2, -0.5, 0, 0.5, 2];

      testCase.verifyEqual(model.evaluate(x), [2, 0.5, 0.245, 0.5, 2], ...
        AbsTol=1e-14)
      testCase.verifyEqual(model.deriv(x), [-1, -1, 0, 1, 1])
      testCase.verifyEqual(model.deriv_over_x(x), ...
        [0.5, 2, 2.040816326530612, 2, 0.5], AbsTol=1e-14)
    end

    function testDisplay(testCase)
      output = evalc('disp(robust_function(''quadratic'', 2))');
      testCase.verifySubstring(output, 'Robust function type:')
      testCase.verifySubstring(output, 'Parameter:')
    end
  end
end
