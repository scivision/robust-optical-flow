function oct = isoctave()
%Michael Hirsch Oct 2012
% tested with Octave 3.6.3/4.0.0 and Matlab R2012a/R2015a

oct = exist('OCTAVE_VERSION', 'builtin') == 5;

end
