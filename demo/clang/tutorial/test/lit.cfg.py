import lit.formats
from lit.llvm import llvm_config

config.name = "clang-tutorial"
config.test_format = lit.formats.ShTest(execute_external=False)
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(config.custom_build_dir, "test")
config.suffixes = [".cpp"]

# llvm settings, use FileCheck and clang it lit RUN command
llvm_config.use_default_substitutions()
# 使用 `use_clang` 方法, 并且需要设置 config.host_triple 以及 config.target_triple
llvm_config.use_clang()

# 映射 %lib
config.substitutions.append(("%lib", config.custom_lib_dir))
config.substitutions.append(("%ext", config.shared_lib_ext))
