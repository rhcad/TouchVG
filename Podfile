platform :ios, '6.0'
xcodeproj 'ios/TouchVG.xcodeproj'

target "TouchVG" do
  pod 'TouchVGCore', '~> 0.29'
  target "TouchVG-SVG" do
    pod 'SVGKit', :git => 'https://github.com/SVGKit/SVGKit.git'
  end
end

post_install do |installer|
  default_library = installer.libraries.detect { |i| i.target_definition.name == 'Pods' }
  config_file_path = default_library.library.xcconfig_path

  File.open("config.tmp", "w") do |io|
    io << File.read(config_file_path).gsub(/-lxml2/, '')
  end
  
  FileUtils.mv("config.tmp", config_file_path)
end