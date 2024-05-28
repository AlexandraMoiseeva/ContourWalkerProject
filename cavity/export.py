from QFormAPI import *
script_dir = os.path.dirname(os.path.abspath(__file__))

try:
	qform = QForm()
	qform.qform_dir_set(r'\\Dmitry\TestRelease\QForm\11.0.224.10\x64')

	if qform.is_started_by_qform():
		qform.qform_attach()
	else:
		arg2 = SessionId()
		arg2.session_id = 0
		qform.qform_attach_to(arg2)

	ret3:Record = qform.record_get_last()
	for i in range(1, int(ret3.record)+1):
		ret3.record = i
		qform.record_set(ret3)
		arg1 = MeshExport()
		arg1.object_type = ObjectType.Workpiece
		arg1.object_id = 1
		arg1.file = script_dir + r'/data/' + '{:03d}'.format(i) + '-wp'
		arg1.format = MeshFormat.CSV2D
		arg1.mesh_index = 0
		arg1.surface_only = True
		qform.export_mesh(arg1)

		arg1.object_type = ObjectType.Tool
		arg1.object_id = 1
		arg1.file = script_dir + r'/data/' + '{:03d}'.format(i) + '-t1'
		qform.export_mesh(arg1)

		arg1.object_id = 2
		arg1.file = script_dir + r'/data/' + '{:03d}'.format(i) + '-t2'
		qform.export_mesh(arg1)

except Exception as ex:
	print(str(ex))

