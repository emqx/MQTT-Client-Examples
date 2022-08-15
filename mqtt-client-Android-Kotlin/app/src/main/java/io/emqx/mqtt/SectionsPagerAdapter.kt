package io.emqx.mqtt

import android.content.Context
import android.view.ViewGroup
import androidx.annotation.StringRes
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager
import androidx.fragment.app.FragmentPagerAdapter

class SectionsPagerAdapter(
    fm: FragmentManager,
    private val mContext: Context,
    private val mFragmentList: List<Fragment>
) : FragmentPagerAdapter(fm) {
    override fun getItem(position: Int): Fragment {
        return mFragmentList[position]
    }

    override fun getPageTitle(position: Int): CharSequence {
        return mContext.resources.getString(TAB_TITLES[position])
    }

    override fun getCount(): Int {
        return mFragmentList.size
    }

    override fun destroyItem(container: ViewGroup, position: Int, `object`: Any) {}

    companion object {
        @StringRes
        private val TAB_TITLES = intArrayOf(
            R.string.connection,
            R.string.subscription,
            R.string.publish,
            R.string.message
        )
    }
}