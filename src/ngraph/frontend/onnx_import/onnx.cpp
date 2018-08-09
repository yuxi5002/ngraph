/*******************************************************************************
 * Copyright 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

#include <fstream>

#include "ngraph/except.hpp"

#include "exceptions.hpp"
#include "graph.hpp"
#include "model.hpp"
#include "node.hpp"
#include "onnx.hpp"
#include "tensor.hpp"

namespace ngraph
{
    template <typename T>
    static inline std::vector<T> _read_tensor_proto_data_file(const std::string& path)
    {
        onnx::TensorProto tensor_proto{path};
        // return tensor_proto.get_data<T>();
        // TODO: implement sth similar to: onnx.numpy_helper.to_array
    }

    namespace onnx_import
    {
        std::vector<std::shared_ptr<Function>> load_onnx_model(std::istream& sin)
        {
            onnx::ModelProto model_proto;
            if (!model_proto.ParseFromIstream(&sin))
            {
                throw error::stream_parse{sin};
            }
            std::vector<std::shared_ptr<Function>> output_functions;
            Model model{model_proto};
            Graph graph{model_proto.graph()};
            for (const auto& output : graph.get_outputs())
            {
                output_functions.emplace_back(std::make_shared<Function>(
                    graph.get_ng_node_from_cache(output.get_name()), graph.get_ng_parameters()));
            }
            return output_functions;
        }

        std::vector<std::shared_ptr<Function>> load_onnx_model(const std::string& path)
        {
            std::ifstream ifs{path, std::ios::in | std::ios::binary};
            if (!ifs.is_open())
            {
                throw error::file_open{path};
            }
            return load_onnx_model(ifs);
        }

        std::shared_ptr<Function> import_onnx_function(std::istream& sin)
        {
            return load_onnx_model(sin).front();
        }

        std::shared_ptr<Function> import_onnx_function(const std::string& path)
        {
            return load_onnx_model(path).front();
        }

        std::vector<double> read_tensor_proto_data_file(const std::string& path)
        {
            return _read_tensor_proto_data_file<double>(path);
        }
    } // namespace onnx_import

} // namespace ngraph
