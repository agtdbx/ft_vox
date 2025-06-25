#ifndef VULKAN_QUEUE_HPP
# define VULKAN_QUEUE_HPP

# include <define.hpp>

struct VulkanQueue
{
	VkQueue		value;
	uint32_t	id;

	VulkanQueue	&operator=(const VulkanQueue &obj)
	{
		if (this == &obj)
			return (*this);

		this->value = obj.value;
		this->id = obj.id;

		return (*this);
	}
};


#endif
