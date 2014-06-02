/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements. The ASF licenses this
file to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.   
*/

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "icurvctl.h"

class BerconRefMaker : public ReferenceMaker {
public:	
	BerconRefMaker(ResourceMakerCallback *m) {thing = m;}		
	ResourceMakerCallback *thing;
	void DeleteThis() {delete this;}
	virtual void* GetInterface(ULONG id) {
		if(id == I_RESMAKER_INTERFACE)
			return (void *) thing;
		else
			return ReferenceMaker::GetInterface(id);
	}
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,RefMessage message){return REF_DONTCARE;}
};