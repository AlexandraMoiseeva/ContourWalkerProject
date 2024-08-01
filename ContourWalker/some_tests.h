#pragma once
#include "Bodies.h"
#include "BodiesSupport.h"
#include "CM_CavityModel2D.h"

#include <cassert>
#include <sstream>
#include <string>

class CM_CavityModelSerializable : public CM_CavityModel2D {
public:
	void Serialize(std::ostream& out) const {
		out << tools.size() << '\n';
		for (const auto& body : tools) {
			WriteBody(body, out);
		}
		out << workpieces.size() << '\n';
		for (const auto& body : workpieces) {
			WriteBody(body, out);
		}
	}

	void SerializeCavities(std::ostream& out) const {
		out << cavities.size() << '\n';
		for (const auto& body : cavities) {
			WriteBody(body, out);
		}
	}
	void DeSerialize(std::istream& in) {
		size_t tools_count;
		in >> tools_count;
		tools.reserve(tools_count);
		while (tools_count--) {
			ReadBody(in, false);
		}
		size_t wp_count;
		in >> wp_count;
		workpieces.reserve(wp_count);
		while (wp_count--) {
			ReadBody(in, true);
		}
	}
private:
	void ReadBody(std::istream& in, bool wp) {
		int id; size_t nodes_count;
		in >> id >> nodes_count;
		auto& body = wp ? workpieces.emplace_back(nodes_count, id) : tools.emplace_back(nodes_count, id);
		body.nodes.resize(nodes_count);
		// copy nodes
		for (Node& node : body.nodes) {
			in >> node.sourceObjInfo.source_body_id
				>> node.sourceObjInfo.mesh_obj_id
				>> node.isSym
				>> node.coordinate.x
				>> node.coordinate.z;
		}
		// contour (order of nodes)
		size_t contour_nodes_count;
		in >> contour_nodes_count;
		body.contour.nodes.reserve(contour_nodes_count);
		while (contour_nodes_count--) {
			int node_id;
			in >> node_id;
			body.addContour(node_id);
		}
		// sym nodes
		size_t sym_nodes_count;
		in >> sym_nodes_count;
		body.symAxisPoints.resize(sym_nodes_count);
		for (int& node_id : body.symAxisPoints) {
			in >> node_id;
		}
		// contacts
		size_t contacts_count;
		in >> contacts_count;
		assert(contacts_count == nodes_count);
		body.contactInit.resize(contacts_count);

		for (auto& contact : body.contactInit) {
			in >> contact.source_body_id
				>> contact.first_point
				>> contact.second_point;
		}
	}

	void WriteBody(const Tool& body, std::ostream& out) const {
		// copy nodes
		out << body.id << '\n';
		out << body.nodes.size() << '\n';
		for (auto& node : body.nodes) {
			out << node.sourceObjInfo.source_body_id
				<< ' ' << node.sourceObjInfo.mesh_obj_id
				<< ' ' << node.isSym
				<< ' ' << node.coordinate.x
				<< ' ' << node.coordinate.z
				<< '\n';
		}
		// contour (order of nodes)
		out << body.contour.nodes.size() << '\n';
		bool first = true;
		for (Node* node : body.contour.nodes) {
			if (!first) {
				out << ' ';
			}
			out << node->sourceObjInfo.mesh_obj_id;
			first = false;
		}
		// sym nodes
		out << '\n' << body.symAxisPoints.size() << '\n';
		first = true;
		for (int node_id : body.symAxisPoints) {
			if (!first) {
				out << ' ';
			}
			out << node_id;
			first = false;
		}
		// contacts
		out << '\n' << body.contactInit.size() << '\n';
		for (const auto& contact : body.contactInit) {
			out << contact.source_body_id
				<< ' ' << contact.first_point
				<< ' ' << contact.second_point
				<< '\n';
		}
	}

	void WriteBody(const Body& body, std::ostream& out) const {
		// copy nodes
		out << body.id << '\n';
		out << body.nodes.size() << '\n';
		for (auto& node : body.nodes) {
			out << node.sourceObjInfo.source_body_id
				<< ' ' << node.sourceObjInfo.mesh_obj_id
				<< ' ' << node.isSym
				<< ' ' << node.coordinate.x
				<< ' ' << node.coordinate.z
				<< '\n';
		}
		// contour (order of nodes)
		out << body.contour.nodes.size() << '\n';
		bool first = true;
		for (Node* node : body.contour.nodes) {
			if (!first) {
				out << ' ';
			}
			out << node->sourceObjInfo.mesh_obj_id;
			first = false;
		}
	}
};

static const std::string SERIALIZED_DATA = R"(
2
1
64
1 0 0 0.08 0.0289225
1 1 0 0.08 0.128923
1 2 0 0.06 0.128923
1 3 0 0.05 0.0489225
1 4 1 0 0.0289225
1 5 1 0 0.0489225
1 6 0 0.005 0.0289225
1 7 0 0.01 0.0289225
1 8 0 0.015 0.0289225
1 9 0 0.02 0.0289225
1 10 0 0.025 0.0289225
1 11 0 0.03 0.0289225
1 12 0 0.035 0.0289225
1 13 0 0.04 0.0289225
1 14 0 0.045 0.0289225
1 15 0 0.05 0.0289225
1 16 0 0.055 0.0289225
1 17 0 0.06 0.0289225
1 18 0 0.065 0.0289225
1 19 0 0.07 0.0289225
1 20 0 0.075 0.0289225
1 21 0 0.08 0.0351725
1 22 0 0.08 0.0414225
1 23 0 0.08 0.0476725
1 24 0 0.08 0.0539225
1 25 0 0.08 0.0601725
1 26 0 0.08 0.0664225
1 27 0 0.08 0.0726725
1 28 0 0.08 0.0789225
1 29 0 0.08 0.0851725
1 30 0 0.08 0.0914225
1 31 0 0.08 0.0976725
1 32 0 0.08 0.103923
1 33 0 0.08 0.110173
1 34 0 0.08 0.116423
1 35 0 0.08 0.122673
1 36 0 0.075 0.128923
1 37 0 0.07 0.128923
1 38 0 0.065 0.128923
1 39 0 0.059375 0.123923
1 40 0 0.05875 0.118923
1 41 0 0.058125 0.113923
1 42 0 0.0575 0.108923
1 43 0 0.056875 0.103923
1 44 0 0.05625 0.0989225
1 45 0 0.055625 0.0939225
1 46 0 0.055 0.0889225
1 47 0 0.054375 0.0839225
1 48 0 0.05375 0.0789225
1 49 0 0.053125 0.0739225
1 50 0 0.0525 0.0689225
1 51 0 0.051875 0.0639225
1 52 0 0.05125 0.0589225
1 53 0 0.050625 0.0539225
1 54 0 0.04375 0.0489225
1 55 0 0.0375 0.0489225
1 56 0 0.03125 0.0489225
1 57 0 0.025 0.0489225
1 58 0 0.01875 0.0489225
1 59 0 0.0125 0.0489225
1 60 0 0.00625 0.0489225
1 61 1 0 0.0439225
1 62 1 0 0.0389225
1 63 1 0 0.0339225
65
60 5 61 62 63 4 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 0 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 1 36 37 38 2 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 3 54 55 56 57 58 59 60
5
4 5 61 62 63
64
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
0 1 48
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
0 18 19
0 16 17
-1 -1 -1
-1 -1 -1
0 7 8
0 5 6
0 2 3
0 53 54
0 51 52
0 49 50
-1 -1 -1
-1 -1 -1
-1 -1 -1
5
20
5 0 0 0.045 0.0839225
5 1 0 0.045 0.0689225
5 2 1 0 0.0839225
5 3 1 0 0.0689225
5 4 0 0.005625 0.0689225
5 5 0 0.01125 0.0689225
5 6 0 0.016875 0.0689225
5 7 0 0.0225 0.0689225
5 8 0 0.028125 0.0689225
5 9 0 0.03375 0.0689225
5 10 0 0.039375 0.0689225
5 11 0 0.045 0.0764225
5 12 0 0.039375 0.0839225
5 13 0 0.03375 0.0839225
5 14 0 0.028125 0.0839225
5 15 0 0.0225 0.0839225
5 16 0 0.016875 0.0839225
5 17 0 0.01125 0.0839225
5 18 0 0.005625 0.0839225
5 19 1 0 0.0764225
21
18 2 19 3 4 5 6 7 8 9 10 1 11 0 12 13 14 15 16 17 18
3
2 3 19
20
-1 -1 -1
0 24 25
-1 -1 -1
0 39 40
0 37 38
0 36 37
0 33 34
0 31 32
0 30 31
0 0 29
0 28 0
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
1
0
55
0 0 0 0.0522541 0.0718475
0 1 1 0 0.0689225
0 2 1 0 0.0489225
0 3 0 0.00171364 0.0489225
0 4 0 0.00522939 0.0489225
0 5 0 0.00743378 0.0489225
0 6 0 0.0105371 0.0489225
0 7 0 0.0141064 0.0489225
0 8 0 0.0159285 0.0489225
0 9 0 0.0196745 0.0489225
0 10 0 0.0219724 0.0489225
0 11 0 0.0251836 0.0489225
0 12 0 0.0288241 0.0489225
0 13 0 0.0306746 0.0489225
0 14 0 0.03433 0.0489225
0 15 0 0.0366334 0.0489225
0 16 0 0.0389588 0.0489225
0 17 0 0.0413617 0.0489225
0 18 0 0.0442314 0.0496475
0 19 0 0.0469044 0.0511337
0 20 0 0.0485335 0.0521717
0 21 0 0.0496434 0.0528832
0 22 0 0.0505374 0.0538004
0 23 0 0.0507884 0.0552299
0 24 0 0.0509966 0.0568955
0 25 0 0.0513631 0.0598274
0 26 0 0.0516426 0.0620632
0 27 0 0.0519256 0.0643272
0 28 0 0.0523851 0.0680037
0 29 0 0.0501722 0.0717843
0 30 0 0.0481259 0.0714345
0 31 0 0.0464942 0.0703333
0 32 0 0.0449204 0.0689225
0 33 0 0.0433229 0.0689225
0 34 0 0.0417284 0.0689225
0 35 0 0.0394749 0.0689225
0 36 0 0.0353859 0.0689225
0 37 0 0.0330902 0.0689225
0 38 0 0.0295831 0.0689225
0 39 0 0.0263766 0.0689225
0 40 0 0.0218155 0.0689225
0 41 0 0.0192335 0.0689225
0 42 0 0.01683 0.0689225
0 43 0 0.0144282 0.0689225
0 44 0 0.0119792 0.0689225
0 45 0 0.00952588 0.0689225
0 46 0 0.00554918 0.0689225
0 47 0 0.00379161 0.0689225
0 48 1 0 0.0661074
0 49 1 0 0.0636292
0 50 1 0 0.0613832
0 51 1 0 0.0591088
0 52 1 0 0.0568741
0 53 1 0 0.0544437
0 54 1 0 0.0518278
56
1 48 49 50 51 52 53 54 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 0 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 1
9
1 2 48 49 50 51 52 53 54
55
-1 -1 -1
5 18 2
1 55 56
1 55 56
1 55 56
1 54 55
1 54 55
1 3 54
1 3 54
1 53 3
1 53 3
1 52 53
1 52 53
1 52 53
1 51 52
1 51 52
1 50 51
1 50 51
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
1 47 48
1 47 48
1 46 47
1 46 47
1 45 46
1 45 46
-1 -1 -1
-1 -1 -1
-1 -1 -1
5 7 8
5 7 8
5 7 8
5 7 8
5 6 7
5 5 6
5 5 6
5 4 5
5 3 4
5 3 4
5 19 3
5 19 3
5 19 3
5 2 19
5 18 2
5 18 2
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
-1 -1 -1
)";

namespace tests {

inline void SomeTests() {
	CM_CavityModelSerializable cm_model;
	std::istringstream is(SERIALIZED_DATA);
	cm_model.DeSerialize(is);
	cm_model.initialisation(2, 1); // crash
	cm_model.findCavity();
	std::ostringstream os;
	cm_model.SerializeCavities(os);
	std::cout << os.str() << std::endl;
}

}
