Pod::Spec.new do |s|
  s.name        = "TouchVG-SVG"
  s.version     = "1.1.11"
  s.summary     = "A lightweight 2D vector drawing framework for iOS."
  s.homepage    = "https://github.com/touchvg/TouchVG"
  s.screenshots = "https://github.com/touchvg/TouchVG/raw/develop/doc/images/iphone1.png"
  s.license     = { :type => "LGPL", :file => "LICENSE.md" }
  s.author      = { "Zhang Yungui" => "rhcad@hotmail.com" }
  s.social_media_url    = "http://weibo.com/rhcad"

  s.platform    = :ios, "5.0"
  s.source      = { :git => "https://github.com/touchvg/TouchVG.git", :branch => "develop" }
  s.ios.source_files    = 'ios/include/*.h', 'ios/src/*.{h,m,mm}'
  s.public_header_files = 'ios/include/*.h'
  s.frameworks = "UIKit", "QuartzCore", "CoreGraphics", "Foundation"

  s.requires_arc = false
  s.xcconfig = {
    'CLANG_CXX_LANGUAGE_STANDARD' => 'gnu++11',
    'CLANG_CXX_LIBRARY' => 'libstdc++',
    "HEADER_SEARCH_PATHS" => '$(PODS_ROOT)/Headers/TouchVGCore $(PODS_ROOT)/Headers/SVGKit'
  }
  s.resource = 'ios/TouchVG.bundle'
  s.dependency "TouchVGCore", "~> 1.0.33"
  s.dependency "SVGKit", :podspec => 'https://raw.githubusercontent.com/SVGKit/SVGKit/2.x/SVGKit.podspec'
end
