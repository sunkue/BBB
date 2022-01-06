#include "stdafx.h"
#include "BO.h"

namespace BO_HELPER
{
	GLuint get_next_binding_point()
	{
		static GLuint bindpoint = 0;
		return bindpoint++;
	}
}
