PROJECT_CEEDLING_ROOT = "vendor/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/ceedling.rb"

Ceedling.load_project

task :default => %w[ test:all release ]

task :coverage do
    Rake::Task["gcov:all"].invoke
    Rake::Task["utils:gcov"].invoke
end

require 'fileutils'
task :publish do
    Rake::Task["release"].invoke
    FileUtils.cp( "build/release/libringer.so.0.0.1", "#{ENV['HOME']}/usr/lib" )
    FileUtils.cp( "src/ringer.h", "#{ENV['HOME']}/usr/include" )
end


task :doxygen do
    sh "doxygen .doxygen"
end

task :coverage do
    Rake::Task["gcov:all"].invoke
    Rake::Task["utils:gcov"].invoke
end

require 'fileutils'
task :publish do
    Rake::Task["release"].invoke
    FileUtils.cp( "build/release/libringer.so.0.0.1", "#{ENV['HOME']}/usr/lib" )
    FileUtils.cp( "src/ringer.h", "#{ENV['HOME']}/usr/include" )
end
