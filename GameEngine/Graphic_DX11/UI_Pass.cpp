#include "DirectDefine.h"
#include "RenderPassBase.h"
#include "ShaderBase.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "GraphicState.h"
#include "GraphicView.h"
#include "GraphicResource.h"
#include "DrawBuffer.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "VertexDefine.h"
#include "EngineData.h"
#include "RenderData.h"
#include "UI_Pass.h"

#include "MathDefine.h"
#include "ResourceFactoryBase.h"
#include "ResourceManagerBase.h"
#include "ShaderManagerBase.h"
#include "ConstantBufferDefine.h"
#include "ShaderResourceViewDefine.h"
#include "DrawBufferDefine.h"
#include "BlendStateDefine.h"

UI_Pass::UI_Pass()
{

}

UI_Pass::~UI_Pass()
{

}

void UI_Pass::Start(int width, int height)
{
	m_UI_VS = g_Shader->GetShader("UI_VS");
	m_UI_PS = g_Shader->GetShader("UI_PS");
	m_UI_Fill_PS = g_Shader->GetShader("UI_Fill_PS");
	m_UI_Fill_Reverse_PS = g_Shader->GetShader("UI_Fill_Reverse_PS");

	// Buffer 설정..
	m_Screen_DB = g_Resource->GetDrawBuffer<DB_Quad>();

	// Graphic View..
	m_OutPut_RTV = g_Resource->GetMainRenderTarget()->GetRTV()->Get();

	// Graphic State..
	m_AlphaBlend_BS = g_Resource->GetBlendState<BS_AlphaBlend_One>()->Get();
}

void UI_Pass::OnResize(int width, int height)
{
	// Graphic View..
	m_OutPut_RTV = g_Resource->GetMainRenderTarget()->GetRTV()->Get();
}

void UI_Pass::Release()
{

}

void UI_Pass::BeginRender()
{
	// Render State Update..
	g_Context->OMSetBlendState(m_AlphaBlend_BS, 0, 0xffffffff);
	g_Context->OMSetRenderTargets(1, &m_OutPut_RTV, nullptr);

	g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_Context->IASetVertexBuffers(0, 1, m_Screen_DB->VertexBuf->GetAddress(), &m_Screen_DB->Stride, &m_Screen_DB->Offset);
	g_Context->IASetIndexBuffer(m_Screen_DB->IndexBuf->Get(), DXGI_FORMAT_R32_UINT, 0);
}

void UI_Pass::RenderUpdate(std::vector<RenderData*>& meshlist)
{
	m_RenderCount = (UINT)meshlist.size();

	if (m_RenderCount == 0) return;

	const Matrix& proj = g_GlobalData->MainCamera_Data->CamOrthoProj;

	const UIRenderBuffer* ui = nullptr;
	const UIProperty* ui_property = nullptr;
	CB_UIObject objectBuf;
	CB_UIOption optionBuf;
	CB_UIFill fillBuf;

	for (UINT i = 0; i < m_RenderCount; i++)
	{
		m_RenderData = meshlist[i];

		ui = m_RenderData->m_UI;
		ui_property = ui->m_UIProperty;

		// 활성화 상태의 UI가 아니라면 그리지 않는다..
		if (ui_property->IsDraw == false) continue;
		if (m_RenderData->m_ObjectData->IsActive == false) continue;


		// Vertex Shader Update..
		objectBuf.gWorldViewProj = ui_property->World * proj;
		m_UI_VS->ConstantBufferUpdate(&objectBuf);

		m_UI_VS->Update();

		// Pixel Shader Update..
		optionBuf.gColor = ui_property->ImageColor;

		switch (ui_property->UI_Option)
		{
		case UI_TYPE::UI_DEFAULT:
		{
			m_UI_PS->ConstantBufferUpdate(&optionBuf);
			m_UI_PS->SetShaderResourceView<gDiffuseMap>(ui->m_Albedo);

			m_UI_PS->Update();
		}
			break;
		case UI_TYPE::UI_FILL:
		{
			fillBuf.gFill = ui_property->TexFill;

			m_UI_Fill_PS->ConstantBufferUpdate(&optionBuf);
			m_UI_Fill_PS->ConstantBufferUpdate(&fillBuf);
			m_UI_Fill_PS->SetShaderResourceView<gDiffuseMap>(ui->m_Albedo);

			m_UI_Fill_PS->Update();
		}
			break;
		case UI_TYPE::UI_FILL_REVERSE:
		{
			fillBuf.gFill = ui_property->TexFill;

			m_UI_Fill_Reverse_PS->ConstantBufferUpdate(&optionBuf);
			m_UI_Fill_Reverse_PS->ConstantBufferUpdate(&fillBuf);
			m_UI_Fill_Reverse_PS->SetShaderResourceView<gDiffuseMap>(ui->m_Albedo);

			m_UI_Fill_Reverse_PS->Update();
		}
		break;
		default:
			break;
		}

		g_Context->DrawIndexed(m_Screen_DB->IndexCount, 0, 0);
	}

}

void UI_Pass::EndRender()
{
	g_Context->OMSetBlendState(0, 0, 0xffffffff);
}
