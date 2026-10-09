classdef alt_ba_optical_flow
%ALT_BA_OPTICAL_FLOW Optical flow computation with the alternative BA method.

% Copyright 2009-2010, Brown University, Providence, RI. USA
%
% All commercial use of this software, whether direct or indirect, is
% strictly prohibited including, without limitation, incorporation into a
% commercial product, use in a commercial service, or production of other
% artifacts for commercial purposes.
%
% Permission to use, copy, modify, and distribute this software and its
% documentation for research purposes is hereby granted without fee,
% provided that the above copyright notice appears in all copies and that
% both that copyright notice and this permission notice appear in
% supporting documentation, and that the name of the author and Brown
% University not be used in advertising or publicity pertaining to
% distribution of the software without specific, written prior permission.
%
% For commercial uses contact the Technology Venture Office of Brown University.
%
% THE AUTHOR AND BROWN UNIVERSITY DISCLAIM ALL WARRANTIES WITH REGARD TO
% THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
% FITNESS FOR ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR OR
% BROWN UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
% DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
% PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
% ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
% THIS SOFTWARE.

properties
  images = []
  lambda = 5
  lambda_q = 5
  sor_max_iters = 1e4
  limit_update = true
  display = false
  solver = 'backslash'
  warping_mode = 'backward'
  texture = false
  deriv_filter = [1 -8 0 8 -1] / 12
  median_filter_size = []
  interpolation_method = 'cubic'
  gnc_iters = 3
  alpha = 1
  max_iters = 10
  max_linear = 1
  pyramid_levels = 4
  pyramid_spacing = 2
  gnc_pyramid_levels = 2
  gnc_pyramid_spacing = 1.25
  spatial_filters = {[1 -1], [1; -1]}
  rho_spatial_u = {}
  rho_spatial_v = {}
  rho_data = []
  seg = []
  mfT = 15
  imfsz = [7 7]
  qterm = true
  lambda2 = 1e-1
  lambda3 = 1
  weightRatio = 1
  itersLO = 1
  color_images = []
  auto_level = true
  replacment = true
  rho_couple = []
end

methods
  function this = alt_ba_optical_flow(imageOrCopy)
    arguments
      imageOrCopy = []
    end

    if isa(imageOrCopy, 'alt_ba_optical_flow')
      this = imageOrCopy;
      return
    end
    this.images = imageOrCopy;

    for i = 1:numel(this.spatial_filters)
      this.rho_spatial_u{i} = robust_function('lorentzian', 0.03);
      this.rho_spatial_v{i} = robust_function('lorentzian', 0.03);
    end
    this.rho_data = robust_function('lorentzian', 1.5);
    this.rho_couple = robust_function('charbonnier', 1e-3);
  end

  function disp(this)
    fprintf('Image sequence (size):          %s\n', mat2str(size(this.images)))
    fprintf('Spatial term weighting:         %s\n', mat2str(this.lambda))
    fprintf('Linear solver:                  %s\n', this.solver)
    fprintf('# of pyramid levels:            %d\n', this.pyramid_levels)
    fprintf('Spacing of pyramid levels:      %d\n', this.pyramid_spacing)
    fprintf('Max. # of iterations:           %d\n', this.max_iters)
    fprintf('Limit flow updates              %d\n', this.limit_update)
  end
end

methods
  uv = compute_flow(this, init, gt)
  [uv, uvhat] = compute_flow_base(this, uv, uvhat)
  L = evalaute_new_log_posterior(this, uv, uvhat)
  L = evaluate_log_posterior(this, uv)
  [A, b, params, iterative] = flow_operator(this, uv, duv, It, Ix, Iy)
  this = parse_input_parameter(this, param)
end
end
