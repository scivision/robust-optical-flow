classdef TestRemainingOpticalFlowClassdef < matlab.unittest.TestCase

  properties
    Images
    InitialFlow
  end

  methods (TestClassSetup)
    function addFlowCodePath(testCase)
      import matlab.unittest.fixtures.PathFixture
      flowCodeDir = fileparts(fileparts(mfilename('fullpath')));
      testCase.applyFixture(PathFixture(flowCodeDir))
      testCase.applyFixture(PathFixture( ...
        fullfile(flowCodeDir, 'utils'), IncludingSubfolders=true))
    end
  end

  methods (TestMethodSetup)
    function createInputs(testCase)
      [x, y] = meshgrid(1:24);
      testCase.Images = cat(3, ...
        sin(x / 4) + cos(y / 5), ...
        sin((x - 0.3) / 4) + cos((y + 0.2) / 5));
      testCase.InitialFlow = cat(3, ...
        0.1 * sin(x / 8), 0.1 * cos(y / 7));
    end
  end

  methods (Test)
    function testConstructorsCopiesAndValueSemantics(testCase)
      names = {'ba_optical_flow', 'alt_ba_optical_flow', ...
        'classic_nl_optical_flow'};
      for i = 1:numel(names)
        factory = str2func(names{i});
        defaultModel = factory();
        imageModel = factory(testCase.Images);
        copiedModel = factory(imageModel);

        testCase.verifyEmpty(defaultModel.images)
        testCase.verifyEqual(imageModel.images, testCase.Images)
        testCase.verifyClass(imageModel.rho_data, 'robust_function')
        copiedModel.lambda = copiedModel.lambda + 1;
        copiedModel.rho_data.param = copiedModel.rho_data.param + 1;
        testCase.verifyNotEqual(copiedModel.lambda, imageModel.lambda)
        testCase.verifyNotEqual(copiedModel.rho_data.param, ...
          imageModel.rho_data.param)
        testCase.verifyError( ...
          @() factory(testCase.Images, testCase.Images), 'MATLAB:TooManyInputs')
      end
    end

    function testBlackAnandanOperatorAndPosterior(testCase)
      model = ba_optical_flow(testCase.Images);
      testCase.verifyNumericalBaseline(model, ...
        159474.1467003523, 100.4235685137027, -50.19280466208676)
    end

    function testAlternativeBlackAnandanOperatorAndPosterior(testCase)
      model = alt_ba_optical_flow(testCase.Images);
      testCase.verifyNumericalBaseline(model, ...
        797369.4713523096, 502.0899824424586, -249.7854104224606)
    end

    function testClassicNonlocalOperatorAndPosterior(testCase)
      model = classic_nl_optical_flow(testCase.Images);
      testCase.verifyNumericalBaseline(model, ...
        164993.8823004304, 21.48434804987254, -46.42114921498157)
    end

    function testOneStepSolverBaselines(testCase)
      baModel = ba_optical_flow(testCase.Images);
      baModel.max_iters = 1;
      baModel.max_linear = 1;
      baModel.gnc_iters = 1;
      baFlow = compute_flow_base(baModel, testCase.InitialFlow);
      testCase.verifyEqual(norm(baFlow(:)), 8.755238033844, Abstol=1e-9)
      testCase.verifyEqual(baFlow(12, 12, 1), ...
        0.303273047240046, Abstol=1e-10)
      testCase.verifyEqual(baFlow(12, 12, 2), ...
        -0.202745281658632, Abstol=1e-10)

      classicModel = classic_nl_optical_flow(testCase.Images);
      classicModel.max_iters = 1;
      classicModel.max_linear = 1;
      classicModel.gnc_iters = 1;
      classicFlow = compute_flow_base(classicModel, testCase.InitialFlow);
      testCase.verifyEqual(norm(classicFlow(:)), ...
        8.7281264157067, Abstol=1e-9)
      testCase.verifyEqual(classicFlow(12, 12, 1), ...
        0.30163493838492, Abstol=1e-10)
      testCase.verifyEqual(classicFlow(12, 12, 2), ...
        -0.202772732341372, Abstol=1e-10)
    end

    function testBlackAnandanSorSolver(testCase)
      baModel = ba_optical_flow(testCase.Images);
      baModel.solver = 'sor';
      baModel.max_iters = 1;
      baModel.max_linear = 1;
      baModel.gnc_iters = 1;
      baFlow = compute_flow_base(baModel, testCase.InitialFlow);
      testCase.verifyTrue(all(isfinite(baFlow(:))))
      testCase.verifyEqual(norm(baFlow(:)), 4.14759515576344, RelTol=1e-6)
      testCase.verifyEqual(baFlow(12, 12, 1), ...
        0.136930428715481, AbsTol=1e-8)
      testCase.verifyEqual(baFlow(12, 12, 2), ...
        -0.105994223784277, AbsTol=1e-8)
    end

    function testAlternativeBlackAnandanSorSolver(testCase)
      altModel = alt_ba_optical_flow(testCase.Images);
      altModel.solver = 'sor';
      altModel.max_iters = 1;
      altModel.max_linear = 1;
      altModel.gnc_iters = 1;
      altModel.median_filter_size = [3 3];
      altFlow = compute_flow_base(altModel, testCase.InitialFlow, ...
        zeros(size(testCase.InitialFlow)));
      testCase.verifyTrue(all(isfinite(altFlow(:))))
      testCase.verifyEqual(norm(altFlow(:)), 2.51059912304811, ...
        RelTol=1e-6)
      testCase.verifyEqual(altFlow(12, 12, 1), ...
        0.104308261985021, AbsTol=1e-8)
      testCase.verifyEqual(altFlow(12, 12, 2), ...
        -0.0132696212922677, AbsTol=1e-8)
    end

    function testClassicNonlocalSorSolver(testCase)
      classicModel = classic_nl_optical_flow(testCase.Images);
      classicModel.solver = 'sor';
      classicModel.max_iters = 1;
      classicModel.max_linear = 1;
      classicModel.gnc_iters = 1;
      classicFlow = compute_flow_base(classicModel, testCase.InitialFlow);
      testCase.verifyTrue(all(isfinite(classicFlow(:))))
      testCase.verifyEqual(norm(classicFlow(:)), ...
        8.72812640671282, RelTol=1e-6)
      testCase.verifyEqual(classicFlow(12, 12, 1), ...
        0.301634938125273, AbsTol=1e-8)
      testCase.verifyEqual(classicFlow(12, 12, 2), ...
        -0.202772732331991, AbsTol=1e-8)
    end

    function testVectorPlotContrastsWithAxesBackground(testCase)
      fig = figure(Visible='off');
      closeFigure = onCleanup(@() close(fig));
      ax = axes(fig);
      flow = cat(3, ones(24), -ones(24));

      ax.Color = [1 1 1];
      ax.ColorOrder = [0.2 0.3 0.4; 0.6 0.7 0.8];
      ax.ColorOrderIndex = 1;
      plotflow(flow);
      quiverObject = findobj(ax, Type='Quiver');
      testCase.verifyEqual(quiverObject.Color, ax.ColorOrder(1, :))
      testCase.verifyTrue(any(quiverObject.UData(:) ~= 0))
      delete(quiverObject)

      ax.Color = [0 0 0];
      ax.ColorOrder = [0.8 0.7 0.6; 0.4 0.3 0.2];
      ax.ColorOrderIndex = 1;
      plotflow(flow);
      quiverObject = findobj(ax, Type='Quiver');
      testCase.verifyEqual(quiverObject.Color, ax.ColorOrder(1, :))
      testCase.verifyTrue(any(quiverObject.UData(:) ~= 0))
    end

    function testParameterParsingAndDisplay(testCase)
      names = {'ba_optical_flow', 'alt_ba_optical_flow', ...
        'classic_nl_optical_flow'};
      for i = 1:numel(names)
        model = feval(names{i}, testCase.Images);
        model = parse_input_parameter(model, { ...
          'lambda', 12.5, 'pyramid_levels', 3, 'pyramid_spacing', 1.5});
        testCase.verifyEqual(model.lambda, 12.5)
        testCase.verifyEqual(model.lambda_q, 12.5)
        testCase.verifyEqual(model.pyramid_levels, 3)
        testCase.verifyEqual(model.pyramid_spacing, 1.5)
        testCase.verifyFalse(model.auto_level)

        output = evalc('disp(model)');
        testCase.verifySubstring(output, 'Image sequence (size)')
        testCase.verifySubstring(output, 'Max. # of iterations')
        output = evalc('display(model)');
        testCase.verifySubstring(output, 'Image sequence (size)')
        output = evalc('model');
        testCase.verifySubstring(output, 'Image sequence (size)')
      end
    end
  end

  methods (Access=private)
    function verifyNumericalBaseline(testCase, model, expectedOperatorNorm, ...
        expectedRightHandSideNorm, expectedPosterior)
      [It, Ix, Iy] = partial_deriv(model.images, testCase.InitialFlow, ...
        model.interpolation_method, model.deriv_filter);
      [A, b] = flow_operator(model, testCase.InitialFlow, ...
        zeros(size(testCase.InitialFlow)), It, Ix, Iy);
      posterior = evaluate_log_posterior(model, testCase.InitialFlow);

      testCase.verifyEqual(norm(A, 'fro'), expectedOperatorNorm, Abstol=1e-7)
      testCase.verifyEqual(norm(b), expectedRightHandSideNorm, Abstol=1e-10)
      testCase.verifyEqual(posterior, expectedPosterior, Abstol=1e-11)
    end
  end
end
