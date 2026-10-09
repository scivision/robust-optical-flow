classdef TestHsOpticalFlow < matlab.unittest.TestCase
    % Run with runtests('flow_code_2010/tests').

properties
  Images
  InitialFlow
  Model
end

methods (TestClassSetup)
  function addFlowCodePath(testCase)
    import matlab.unittest.fixtures.PathFixture
    flowCodeDir = fileparts(fileparts(mfilename('fullpath')));
    testCase.applyFixture(PathFixture(flowCodeDir))
    testCase.applyFixture(PathFixture( ...
        fullfile(flowCodeDir, 'utils'), IncludingSubfolders=true));
  end
end

methods (TestMethodSetup)
  function createFixture(testCase)
    [x, y] = meshgrid(1:24);
    testCase.Images = cat(3, ...
        sin(x / 4) + cos(y / 5), ...
        sin((x - 0.3) / 4) + cos((y + 0.2) / 5));
    testCase.InitialFlow = cat(3, ...
        0.1 * sin(x / 8), 0.1 * cos(y / 7));
    testCase.Model = hs_optical_flow(testCase.Images);
    testCase.Model.max_warping_iters = 2;
  end
end

methods (Test)
function testDefaultImageAndCopyConstruction(testCase)
defaults = hs_optical_flow;
testCase.verifyEmpty(defaults.images)
testCase.verifyEqual(defaults.lambda, 80)
testCase.verifyEqual(defaults.pyramid_levels, 4)
testCase.verifyEqual(defaults.pyramid_spacing, 2)
testCase.verifyEqual(defaults.max_warping_iters, 10)
testCase.verifyClass(defaults.rho_data, 'robust_function')
testCase.verifyEqual(defaults.rho_data.param, 1)

imageModel = hs_optical_flow(testCase.Images);
testCase.verifyEqual(imageModel.images, testCase.Images)

copiedModel = hs_optical_flow(testCase.Model);
copiedModel.lambda = 23;
copiedModel.rho_data.param = 2;
testCase.verifyEqual(testCase.Model.lambda, 80)
testCase.verifyEqual(testCase.Model.rho_data.param, 1)
testCase.verifyEqual(copiedModel.lambda, 23)
testCase.verifyEqual(copiedModel.rho_data.param, 2)
end

function testParameterParsing(testCase)
model = parse_input_parameter(testCase.Model, { ...
    'lambda', 12.5, ...
    'pyramid_levels', 3, ...
    'pyramid_spacing', 1.5, ...
    'median_filter_size', 3, ...
    'max_warping_iters', 4});

testCase.verifyEqual(model.lambda, 12.5)
testCase.verifyEqual(model.pyramid_levels, 3)
testCase.verifyEqual(model.pyramid_spacing, 1.5)
testCase.verifyEqual(model.median_filter_size, [3 3])
testCase.verifyEqual(model.max_warping_iters, 4)
testCase.verifyEqual(model.max_iters, 4)
end

function testDisplay(testCase)
output = evalc('disp(testCase.Model)');
testCase.verifySubstring(output, 'Image sequence (size)')
testCase.verifySubstring(output, 'Max. # of iterations')

output = evalc('display(testCase.Model)');
testCase.verifySubstring(output, 'Image sequence (size)')

output = evalc('testCase.Model');
testCase.verifySubstring(output, 'Image sequence (size)')
end

function testFlowOperatorMatchesLegacyBaseline(testCase)
[A, b, params, iterative] = flow_operator(testCase.Model, testCase.InitialFlow);

testCase.verifyEqual(size(A), [2 * numel(testCase.Images(:, :, 1)), ...
    2 * numel(testCase.Images(:, :, 1))])
testCase.verifyEmpty(params)
testCase.verifyTrue(iterative)
testCase.verifyEqual(norm(A, 'fro'), 11736.466336665242, Abstol=1e-8)
testCase.verifyEqual(norm(b), 7.811227640825876, Abstol=1e-10)
end

function testPosteriorAndGradientMatchLegacyBaseline(testCase)
energy = evaluate_log_posterior(testCase.Model, testCase.InitialFlow);
gradient = evaluate_log_posterior_grad(testCase.Model, testCase.InitialFlow);

testCase.verifyEqual(energy, -8.774943907392721, Abstol=1e-12)
testCase.verifyEqual(norm(gradient(:)), 15.622455281651749, Abstol=1e-10)
testCase.verifyEqual(gradient(1, 1, 1), 1.9872587700835593, Abstol=1e-12)
testCase.verifyEqual(gradient(12, 12, 2), 0.05321850718267515, Abstol=1e-12)
end

function testFlowSolversMatchLegacyBaseline(testCase)
baseFlow = compute_flow_base(testCase.Model, ...
    testCase.InitialFlow);
testCase.verifyEqual(norm(baseFlow(:)), 8.673245681443815, Abstol=1e-9)

flow = compute_flow(testCase.Model);
testCase.verifyEqual(norm(flow(:)), 8.636961983911194, Abstol=1e-9)
testCase.verifyEqual(flow(12, 12, 1), 0.3008784109935184, Abstol=1e-10)
testCase.verifyEqual(flow(12, 12, 2), -0.20061114708250027, Abstol=1e-10)

model = testCase.Model;
model.median_filter_size = [3 3];
medianFlow = compute_flow(model);
testCase.verifyEqual(norm(medianFlow(:)), 8.649108922858565, Abstol=1e-9)
testCase.verifyEqual(medianFlow(12, 12, 1), 0.30089887956361755, Abstol=1e-10)
end
end
end
