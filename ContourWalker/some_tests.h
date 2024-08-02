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

struct TestData {
	static const std::string DATA1;
	static const std::string DATA2;
};

namespace tests {

inline void SomeTests() {
	{
		CM_CavityModelSerializable cm_model;
		std::istringstream is(TestData::DATA1);
		cm_model.DeSerialize(is);
		cm_model.initialisation(2, 1);
		cm_model.findCavity();
	}
	{
		CM_CavityModelSerializable cm_model;
		std::istringstream is(TestData::DATA2);
		cm_model.DeSerialize(is);
		cm_model.initialisation(2, 1);
		cm_model.findCavity();
	}
}

}
