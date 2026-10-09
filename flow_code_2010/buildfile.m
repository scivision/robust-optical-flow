function plan = buildfile
plan = buildplan;
plan.DefaultTasks = "test";

mexFolder = fullfile(plan.RootFolder, 'utils', 'mex');

plan("sor") = matlab.buildtool.tasks.MexTask( ...
  fullfile(mexFolder, 'sor.cpp'), mexFolder, ...
  Filename="sor", Options="-largeArrayDims");
plan("test") = matlab.buildtool.tasks.TestTask(fullfile(plan.RootFolder, 'tests'));
plan("test").Dependencies = "sor";
plan("clean") = matlab.buildtool.tasks.CleanTask;
end
