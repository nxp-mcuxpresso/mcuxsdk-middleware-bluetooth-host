# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

#  Content   : This class implements a __repr__ function which is used
#              by multiple other classes. It is intended to be used as
#              mixin class.

import numpy as np

class ReprMixin:
    def __repr__(self, level=0):
        prefix = '\t' * level
        s = '%s%s:\n' % (prefix, self.__class__.__name__)

        prefix = '\t' * (level + 1)
        variables = dir(self)
        for var in variables:
            value = getattr(self, var)
            if not callable(value) and not var.startswith('_'):
                try:
                    if isinstance(value, np.ndarray) and value.size > 8:
                        s += '%s%s = %s-array\n' % (prefix, var, str(value.shape))  # for ndarrays with more than 8 elements, just display the dize of the array
                    else:
                        try:
                            s += '%s' % (value.__repr__(level=level+1))     # Use this when the class uses this __repr__ function
                        except:
                            s += '%s%s = %s\n' % (prefix, var, str(value))  # Use this for any regular __repr__ function
                except:
                    pass
        return s
