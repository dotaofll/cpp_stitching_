#pragma once

#include <array>
#include <string_view>
#include <tuple>
namespace Constant
{
constexpr auto DESCRIPTOR_SIZE = 5;
constexpr auto MATCHING_Y_RANGE = 30;
constexpr auto RANSAC_K = 1000;
constexpr auto RANSAC_THRES_DISTANCE = 3;
constexpr auto FEATURE_CUT_X_EDGE = 50;
constexpr auto FEATURE_CUT_Y_EDGE = 50;
constexpr auto WIDTH = 640;
constexpr auto HEIGHT = 480;
constexpr auto FOCAL = 720;

struct CAM_ID
{
	constexpr static std::array<std::string_view, 4> cam_ID{
		"/dev/v4l/by-path/pci-0000:00:14.0-usb-0:3:1.0-video-index0",
		"/dev/v4l/by-path/pci-0000:00:14.0-usb-0:5:1.0-video-index0",
		"/dev/v4l/by-path/pci-0000:05:00.0-usb-0:1:1.0-video-index0",
		"/dev/v4l/by-path/pci-0000:00:14.0-usb-0:4:1.0-video-index0",
	};
};
struct SHIFT_LIST
{
	constexpr static std::array<std::tuple<int, int>, 3> shift_LIST{
		std::make_tuple(-10, -399),
		std::make_tuple(32, -409),
		std::make_tuple(-1, -405)};
};
} // namespace ConstVariable