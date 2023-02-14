
#include <iostream>
#include <string>
#include <vector>
// Need unix stat to check major/minor numbers
#include <sys/stat.h>



using string = std::string;

// Boost Program options
#include <boost/program_options.hpp>
// Boost filesystem
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

#include <version.h>

// Major number of all DRM character devices.
#define DRI_DEVICE_MAJOR 226


// Class to hold the command line options
class Options {
public:
    Options() : help(false), version(false), inputFileName() {}

    // -h, --help option
    bool help;
    // -v, --version option
    bool version;
    // Input file name
    string inputFileName;
    // Path to DRM device
    string drmDevice;
    
};

// Parse the command line options
Options parseOptions(int argc, char** argv) {
    Options options;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "print version string")        
        ("input-file,i", po::value<string>(), "input file")
        ("device,d", po::value<string>(), "path to DRM device")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        options.help = true;
    }

    if (vm.count("version")) {
        std::cout << "kms_image version " << KMS_IMAGE_VERSION << std::endl;
        options.version = true;
    }

    if (vm.count("input-file")) {
        options.inputFileName = vm["input-file"].as<string>();        
    }

    if (vm.count("device")) {
        options.drmDevice = vm["device"].as<string>();        
    }

    return options;
}

// Class to wrap an open DRM device
class DrmDevice {
public:
    DrmDevice(const string& path) : m_path(path) {
        // Open DRM device
        m_fd = open(m_path.c_str(), O_RDWR | O_CLOEXEC);
        if (m_fd < 0) {
            throw std::runtime_error("Failed to open DRM device '" + m_path + "'");
        }
    }

    ~DrmDevice() {
        // Close DRM device
        close(m_fd);
    }
    // Rule of five
    DrmDevice(const DrmDevice&) = delete;
    DrmDevice(DrmDevice&&) = delete;
    DrmDevice& operator=(const DrmDevice&) = delete;
    DrmDevice& operator=(DrmDevice&&) = delete;


private:
    // Path to DRM device
    string m_path;
    // File descriptor of DRM device
    int m_fd;

}

int main(int argc, char** argv) {

    // Check if we have root permissions
    if (geteuid() != 0) {
        std::cout << "You need root permissions to run this program" << std::endl;
        return 1;
    }

    Options options = parseOptions(argc, argv);    

    if (options.help) {
        return 0;
    }

    if (options.version) {
        return 0;
    }

    if (options.inputFileName.empty()) {
        std::cout << "No input file specified" << std::endl;
        return 1;
    }

    if (options.drmDevice.empty()) {
        std::cout << "No DRM device specified" << std::endl;
        return 1;
    }

    std::cout << "Input file: " << options.inputFileName << std::endl;
    std::cout << "DRM device: " << options.drmDevice << std::endl;

    // Sanity checks

    // We collect all errors and print them at the end so the user can fix all of them at once
    std::vector<string> errors;

    // Check input file
    if (!fs::exists(options.inputFileName)) {
        errors.push_back("Input file '" + options.inputFileName + "' does not exist");
    }
    else if (!fs::is_regular_file(options.inputFileName)) {
        errors.push_back("Input file '" + options.inputFileName + "' is not a regular file");
    }
    else if (!fs::status(options.inputFileName).permissions() & fs::owner_read) {
        errors.push_back("Input file '" + options.inputFileName + "' is not readable");
    }

    // Check DRM device
    struct stat drmDeviceStat;
    if (stat(options.drmDevice.c_str(), &drmDeviceStat) != 0) {
        errors.push_back("DRM device '" + options.drmDevice + "' does not exist");
    }
    else if (!S_ISCHR(drmDeviceStat.st_mode)) {
        errors.push_back("DRM device '" + options.drmDevice + "' is not a character device");
    }
    else if (major(drmDeviceStat.st_rdev) != DRI_DEVICE_MAJOR) {
        errors.push_back("DRM device '" + options.drmDevice + "' is not a DRM device");
    }
    
    if (!errors.empty()) {
        for (auto& error : errors) {
            std::cout << error << std::endl;
        }
        return 1;
    }

    // We might be good to go...

    // Open drm device



    return 0;
}
